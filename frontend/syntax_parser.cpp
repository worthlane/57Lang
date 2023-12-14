#include "string.h"

#include "syntax_parser.h"
#include "analysis.h"
#include "tree/tree.h"
#include "common/errors.h"

#ifdef  CUR_PTR
#undef  CUR_PTR
#endif
#define CUR_PTR storage->ptr

#ifdef  INCREASE_PTR
#undef  INCREASE_PTR
#endif
#define INCREASE_PTR     \
                        PrintLog("GOT TOKEN[%d], READING NEXT ON LINE [%d]<br>\n",       \
                                        storage->ptr++,                __LINE__);

#ifdef  DECREASE_PTR
#undef  DECREASE_PTR
#endif
#define DECREASE_PTR    PrintLog("RETURN FROM TOKEN[%d], READING PREVIOUS ON LINE [%d]<br>\n",       \
                                                    storage->ptr++,                __LINE__);

#ifdef  CUR_TOKEN
#undef  CUR_TOKEN
#endif
#define CUR_TOKEN       storage->lexis->tokens[CUR_PTR]

#ifdef  TOKEN_KEYWORD
#undef  TOKEN_KEYWORD
#endif
#define TOKEN_KEYWORD   storage->lexis->names.list[CUR_TOKEN.info.var].name


#ifdef  SYN_ASSERT
#undef  SYN_ASSERT
#endif
#define SYN_ASSERT(stat)                                                                            \
        if (!(stat))                                                                                \
        {                                                                                           \
            LOG_START(__func__, __FILE__, __LINE__);                                                \
            error->code = (int) FrontendErrors::INVALID_SYNTAX;                                     \
            SetErrorData(error, "SYNTAX ASSERT\"" #stat "\"<br>\n"                                  \
                                "IN FUNCTION %s FROM FILE \"%s\"(%d)<br>\n",                        \
                                __func__, __FILE__, __LINE__);                                      \
            LOG_END();                                                                              \
            return nullptr;                                                                         \
        }

// TODO errors


// Body    ::= {Line}*
// Line    ::= Asgn BREAK

// WhileSection ::= WHILE L_BRACKET Expr R_BRACKET OPEN_BLOCK Body CLOSE_BLOCK
// IfSection ::= IF L_BRACKET Expr R_BRACKET OPEN_BLOCK Body CLOSE_BLOCK (ELSE OPEN_BLOCK Body CLOSE_BLOCK)
// Asgn      ::= Var ASSIGN Expr
// Expr      ::= OrOp  { [OR] OrOp }*
// OrOp      ::= AndOp { [AND] AndOp }*
// AndOp     ::= Compare { [<=>] Compare }*
// Compare   ::= T { [ADD SUB] T }*
// T         ::= Deg { [MUL DIV] Deg }*
// Deg       ::= S { [^] S }*
// S         ::= { [SIN COS] P }*
// P         ::= L_BRACKET Expr R_BRACKET | Num

static Node* GetAssign(SyntaxStorage* storage, error_t* error);
static Node* GetExpr(SyntaxStorage* storage, error_t* error);
static Node* GetCompare(SyntaxStorage* storage, error_t* error);
static Node* GetAndOperand(SyntaxStorage* storage, error_t* error);
static Node* GetOrOperand(SyntaxStorage* storage, error_t* error);
static Node* GetT(SyntaxStorage* storage, error_t* error);
static Node* GetDeg(SyntaxStorage* storage, error_t* error);
static Node* GetS(SyntaxStorage* storage, error_t* error);
static Node* GetP(SyntaxStorage* storage, error_t* error);
static Node* GetNum(SyntaxStorage* storage, error_t* error);

// -------------------------------------------------------------

void GetTreeFromTokens(LexisStorage* lexis, tree_t* tree, error_t* error)
{
    assert(lexis);
    assert(tree);
    assert(error);

    SyntaxStorage syn = {};
    syn.ptr = 0;
    syn.lexis = lexis;

    tree->root = GetAssign(&syn, error);
}

// -------------------------------------------------------------

static Node* GetAssign(SyntaxStorage* storage, error_t* error)
{
    assert(storage);
    assert(error);

    SYN_ASSERT(CUR_TOKEN.type == TokenType::VAR);

    Node* var = MakeNode(NodeType::VAR, {.var = CUR_TOKEN.info.var});
    INCREASE_PTR;

    Operators opt = TranslateKeywordToOperator(TOKEN_KEYWORD);

    SYN_ASSERT(CUR_TOKEN.type == TokenType::KEYWORD && opt == Operators::ASSIGN)
    INCREASE_PTR;

    Node* val = GetExpr(storage, error);
    if (error->code != (int) ERRORS::NONE) return nullptr;

    SYN_ASSERT(CUR_TOKEN.type     == TokenType::OP &&
               CUR_TOKEN.info.opt == Operators::BREAK);

    return MakeNode(NodeType::OP, {.opt = opt}, var, val);
}

// -------------------------------------------------------------

static Node* GetExpr(SyntaxStorage* storage, error_t* error)
{
    assert(error);
    assert(storage);

    Node* val = GetOrOperand(storage, error);

    Operators opt = TranslateKeywordToOperator(TOKEN_KEYWORD);

    while (CUR_TOKEN.type == TokenType::KEYWORD && opt == Operators::OR)
    {
        Node* op = MakeNode(NodeType::OP, {.opt = opt});
        INCREASE_PTR;

        Node* val2 = GetOrOperand(storage, error);

        val = ConnectNodes(op, val, val2);

        opt = TranslateKeywordToOperator(TOKEN_KEYWORD);
    }

    return val;
}


// -------------------------------------------------------------

static Node* GetOrOperand(SyntaxStorage* storage, error_t* error)
{
    assert(error);
    assert(storage);

    Node* val = GetAndOperand(storage, error);

    Operators opt = TranslateKeywordToOperator(TOKEN_KEYWORD);

    while (CUR_TOKEN.type == TokenType::KEYWORD && opt == Operators::AND)
    {
        DumpToken(stdout, &CUR_TOKEN);

        Node* op = MakeNode(NodeType::OP, {.opt = opt});
        INCREASE_PTR;

        Node* val2 = GetAndOperand(storage, error);

        val = ConnectNodes(op, val, val2);

        opt = TranslateKeywordToOperator(TOKEN_KEYWORD);
    }

    return val;
}

// -------------------------------------------------------------

static Node* GetAndOperand(SyntaxStorage* storage, error_t* error)
{
    assert(error);
    assert(storage);

    Node* val = GetCompare(storage, error);

    Operators opt = TranslateKeywordToOperator(TOKEN_KEYWORD);

    if (CUR_TOKEN.type == TokenType::KEYWORD &&
       (opt == Operators::LESS    || opt == Operators::LESSEQUAL    || opt == Operators::EQUAL ||
        opt == Operators::GREATER || opt == Operators::GREATEREQUAL || opt == Operators::NOT_EQUAL))
    {
        Node* op = MakeNode(NodeType::OP, {.opt = opt});
        INCREASE_PTR;
        Node* val2 = GetCompare(storage, error);

        val = ConnectNodes(op, val, val2);

        opt = TranslateKeywordToOperator(TOKEN_KEYWORD);
    }

    return val;
}

// -------------------------------------------------------------

static Node* GetCompare(SyntaxStorage* storage, error_t* error)
{
    assert(storage);
    assert(error);

    Node* val = GetT(storage, error);

    while (CUR_TOKEN.type     == TokenType::OP  &&
          (CUR_TOKEN.info.opt == Operators::ADD ||
           CUR_TOKEN.info.opt == Operators::SUB))
    {
        Node* op = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
        INCREASE_PTR;
        Node* val2 = GetT(storage, error);

        val = ConnectNodes(op, val, val2);
    }
    return val;
}

// -------------------------------------------------------------

static Node* GetT(SyntaxStorage* storage, error_t* error)
{
    assert(error);
    assert(storage);

    Node* val = GetDeg(storage, error);

    while (CUR_TOKEN.type     == TokenType::OP  &&
          (CUR_TOKEN.info.opt == Operators::DIV ||
           CUR_TOKEN.info.opt == Operators::MUL))
    {
        Node* op = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
        INCREASE_PTR;
        Node* val2 = GetDeg(storage, error);

        val = ConnectNodes(op, val, val2);
    }
    return val;
}

// -------------------------------------------------------------

static Node* GetDeg(SyntaxStorage* storage, error_t* error)
{
    assert(error);
    assert(storage);

    Node* val = GetS(storage, error);

    while (CUR_TOKEN.type     == TokenType::OP &&
           CUR_TOKEN.info.opt == Operators::DEG)
    {
        Node* op = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
        INCREASE_PTR;
        Node* val2 = GetS(storage, error);

        val = ConnectNodes(op, val, val2);
    }
    return val;
}

// -------------------------------------------------------------

static Node* GetS(SyntaxStorage* storage, error_t* error)
{
    assert(error);
    assert(storage);

    Operators opt = TranslateKeywordToOperator(TOKEN_KEYWORD);

    if (CUR_TOKEN.type == TokenType::KEYWORD &&
       (           opt == Operators::SIN     ||
                   opt == Operators::COS))
    {
        Node* op = MakeNode(NodeType::OP, {.opt = opt});
        INCREASE_PTR;
        Node* val = GetP(storage, error);

        val = ConnectNodes(op, val, nullptr);

        return val;
    }

    return GetP(storage, error);
}

// -------------------------------------------------------------

static Node* GetP(SyntaxStorage* storage, error_t* error)
{
    assert(error);
    assert(storage);


    if (CUR_TOKEN.type     == TokenType::OP &&
        CUR_TOKEN.info.opt == Operators::L_BRACKET)
    {
        INCREASE_PTR;
        Node* val = GetExpr(storage, error);

        SYN_ASSERT(CUR_TOKEN.type     == TokenType::OP &&
                   CUR_TOKEN.info.opt == Operators::R_BRACKET);

        INCREASE_PTR;

        return val;
    }

    return GetNum(storage, error);
}

// -------------------------------------------------------------

static Node* GetNum(SyntaxStorage* storage, error_t* error)
{
    assert(error);
    assert(storage);

    Node* val = nullptr;

    if  (CUR_TOKEN.type     == TokenType::OP  &&
        (CUR_TOKEN.info.opt == Operators::ADD ||
         CUR_TOKEN.info.opt == Operators::SUB))
    {
        Node* op = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
        INCREASE_PTR;

        SYN_ASSERT(CUR_TOKEN.type == TokenType::NUM || CUR_TOKEN.type == TokenType::VAR);

        Node* num = MakeNode(NodeType::NUM, {.val = CUR_TOKEN.info.val});
        INCREASE_PTR;

        val = ConnectNodes(op, MakeNode(NodeType::NUM, {.val = 0}), num);
    }
    else
    {
        SYN_ASSERT(CUR_TOKEN.type == TokenType::NUM || CUR_TOKEN.type == TokenType::VAR);

        Node* num = MakeNode(NodeType::NUM, {.val = CUR_TOKEN.info.val});
        INCREASE_PTR;

        val = num;
    }

    return val;
}

