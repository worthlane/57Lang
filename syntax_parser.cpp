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
#define INCREASE_PTR    PrintLog("TOKEN[%d] GOT SUCCESSFULLY, READING NEXT ON LINE [%d]<br>\n",       \
                                        storage->ptr++,                          __LINE__);

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
            error->code = (int) ERRORS::READ_FILE;                                   \
            LOG_START(__func__, __FILE__, __LINE__);                                                \
            PrintLog("SYNTAX ASSERT\"" #stat "\"<br>\n"                                             \
                     "IN FUNCTION %s FROM FILE \"%s\"(%d)<br>\n", __func__, __FILE__, __LINE__);    \
            LOG_END();                                                                              \
            return nullptr;                                                                         \
        }

// TODO errors

// Asgn    ::= Var ASSIGN Expr END
// Expr    ::= T { [ADD SUB] T }*
// T       ::= Deg { [MUL DIV] Deg }*
// Deg     ::= S { [^] S }*
// S       ::= { [SIN COS] P }*
// P       ::= L_BRACKET E R_BRACKET | Num

static Node* GetExpr(SyntaxStorage* storage, error_t* error);
static Node* GetT(SyntaxStorage* storage, error_t* error);
static Node* GetDeg(SyntaxStorage* storage, error_t* error);
static Node* GetS(SyntaxStorage* storage, error_t* error);
static Node* GetP(SyntaxStorage* storage, error_t* error);
static Node* GetNum(SyntaxStorage* storage, error_t* error);

// -------------------------------------------------------------

Node* GetAssign(SyntaxStorage* storage, error_t* error)
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
               CUR_TOKEN.info.opt == Operators::BREAK); // TODO make end

    return MakeNode(NodeType::OP, {.opt = opt}, var, val);
}

// -------------------------------------------------------------

static Node* GetExpr(SyntaxStorage* storage, error_t* error)
{
    assert(storage);
    assert(error);

    Node* val = GetT(storage, error);

    while   (CUR_TOKEN.type     == TokenType::OP  &&
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

        DumpToken(stdout, &CUR_TOKEN);

        val = num;
    }

    return val;
}
