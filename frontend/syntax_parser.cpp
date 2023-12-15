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
#define INCREASE_PTR    PrintLog("GOT TOKEN[%d], READING NEXT ON LINE [%d]<br>\n",       \
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

#ifdef  SKIP_BREAKS
#undef  SKIP_BREAKS
#endif
#define SKIP_BREAKS     while(CUR_TOKEN.info.opt == Operators::BREAK && CUR_TOKEN.type == TokenType::OP)       \
                        {                                                   \
                            INCREASE_PTR;                                   \
                        }                                                   \

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

#ifdef  BREAK_ASSERT
#undef  BREAK_ASSERT
#endif
#define BREAK_ASSERT(stat)                                                                            \
        if (!(stat))                                                                                \
        {                                                                                           \
            LOG_START(__func__, __FILE__, __LINE__);                                                \
            PrintLog("LEAVING FUNC BECAUSE OF\"" #stat "\"<br>\n"                                  \
                     "IN FUNCTION %s, FILE \"%s\"(%d)<br>\n",                        \
                                __func__, __FILE__, __LINE__);                                      \
            LOG_END();                                                                              \
            return nullptr;                                                                         \
        }

// GetInit          ::= TYPE L_BRACKET VAR R_BRACKET ASSIGN Expression



// Program          ::= Block END
// WhileSection     ::= WHILE L_BRACKET Expression R_BRACKET {BREAK}* Block CLOSE_BLOCK GetBreak
// IfSection        ::= IF    L_BRACKET Expression R_BRACKET {BREAK}* Block CLOSE_BLOCK GetBreak
// Block            ::= {Line}*
// Line             ::= {IfSection | WhileSection | Return | Assignment }*
// Assignment       ::= VAR ASSIGN Expression GetBreak
// Return           ::= RETURN Expression GetBreak
// Expression       ::= OrOperand    { [OR] OrOperand }*
// OrOperand        ::= AndOperand   { [AND] AndOperand }*
// AndOperand       ::= Comparison   { [<=>] Comparison }*      TODO rename
// Comparison       ::= Term         { [ADD SUB] Term }*
// Term             ::= Degree       { [MUL DIV] Degree }*
// Degree           ::= Trigonometry { [^] Trigonometry }*
// Trigonometry     ::=              { [SIN COS] Brackets }*
// Brackets         ::= L_BRACKET Expression R_BRACKET | Number
// GetType          ::= TYPE
// GetBreak         ::= BREAK
// GetVar           ::= VAR

static Node* GetProgram(SyntaxStorage* storage, error_t* error);

static Node* GetVar(SyntaxStorage* storage, error_t* error);
static Node* GetBreak(SyntaxStorage* storage, error_t* error);
static Node* GetType(SyntaxStorage* storage, error_t* error);
static Node* GetBlock(SyntaxStorage* storage, error_t* error);
static Node* GetLine(SyntaxStorage* storage, error_t* error);
static Node* GetReturn(SyntaxStorage* storage, error_t* error);
static Node* GetWhileSection(SyntaxStorage* storage, error_t* error);
static Node* GetIfSection(SyntaxStorage* storage, error_t* error);
static Node* GetAssignment(SyntaxStorage* storage, error_t* error);
static Node* GetExpression(SyntaxStorage* storage, error_t* error);
static Node* GetComparison(SyntaxStorage* storage, error_t* error);
static Node* GetAndOperand(SyntaxStorage* storage, error_t* error);
static Node* GetOrOperand(SyntaxStorage* storage, error_t* error);
static Node* GetTerm(SyntaxStorage* storage, error_t* error);
static Node* GetDegree(SyntaxStorage* storage, error_t* error);
static Node* GetTrigonometry(SyntaxStorage* storage, error_t* error);
static Node* GetBrackets(SyntaxStorage* storage, error_t* error);
static Node* GetNumber(SyntaxStorage* storage, error_t* error);

// -------------------------------------------------------------

void GetTreeFromTokens(LexisStorage* lexis, tree_t* tree, error_t* error)
{
    assert(lexis);
    assert(tree);
    assert(error);

    SyntaxStorage syn = {};
    syn.ptr = 0;
    syn.lexis = lexis;

    tree->root = GetProgram(&syn, error);

}

// -------------------------------------------------------------

static Node* GetBreak(SyntaxStorage* storage, error_t* error)
{
    SYN_ASSERT(CUR_TOKEN.type     == TokenType::OP &&
               CUR_TOKEN.info.opt == Operators::BREAK);
    INCREASE_PTR;
    Node* _break = MakeNode(NodeType::OP, {.opt = Operators::BREAK});

    return _break;
}

// -------------------------------------------------------------

static Node* GetType(SyntaxStorage* storage, error_t* error)
{
    Operators opt = TranslateKeywordToOperator(TOKEN_KEYWORD);

    SYN_ASSERT(IsType(opt) && CUR_TOKEN.type == TokenType::KEYWORD);
    Node* type = MakeNode(NodeType::OP, {.opt = opt});
    INCREASE_PTR;

    return type;
}

// -------------------------------------------------------------

static Node* GetVar(SyntaxStorage* storage, error_t* error)
{
    assert(storage);
    assert(error);

    SYN_ASSERT(CUR_TOKEN.type == TokenType::VAR);
    Node* var = MakeNode(NodeType::VAR, {.var = CUR_TOKEN.info.var});
    INCREASE_PTR;

    return var;
}

// -------------------------------------------------------------

/*static Node* GetInit(SyntaxStorage* storage, error_t* error)
{
    Node* type = GetType(storage, error);
    if (!error->code) return nullptr;

    Node* fictive_connect = MakeNode(NodeType::OP, {.opt = Operators::TYPE});


}*/


// -------------------------------------------------------------

static Node* GetProgram(SyntaxStorage* storage, error_t* error)
{
    assert(storage);
    assert(error);

    SKIP_BREAKS;

    Node* result = GetBlock(storage, error);


    Operators opt = TranslateKeywordToOperator(TOKEN_KEYWORD);

    SYN_ASSERT(opt == Operators::END && CUR_TOKEN.type == TokenType::KEYWORD);

    return result;
}

// -------------------------------------------------------------

static Node* GetBlock(SyntaxStorage* storage, error_t* error)
{
    assert(storage);
    assert(error);

    Node* val  = GetLine(storage, error);


    Node* head = val;

    while (val != nullptr)
    {
        Node* val2 = GetLine(storage, error);


        ConnectNodes(val, nullptr, val2);
        val = val2;
    }

    return head;
}

// -------------------------------------------------------------

static Node* GetLine(SyntaxStorage* storage, error_t* error)
{
    assert(storage);
    assert(error);

    Node* val = nullptr;

    SKIP_BREAKS;

    Operators opt = TranslateKeywordToOperator(TOKEN_KEYWORD);

    // TODO if (peek(storagr, TokenType::IF))
    // TODO if (PEEK(TokenType::IF))
    // TODO if (storage->peek(TokenType::IF))
    if (opt == Operators::IF && CUR_TOKEN.type == TokenType::KEYWORD)
    {
        Node* _if = GetIfSection(storage, error);

        return _if;
    }

    if (opt == Operators::WHILE && CUR_TOKEN.type == TokenType::KEYWORD)
    {
        Node* _while = GetWhileSection(storage, error);

        return _while;
    }

    if (opt == Operators::RETURN && CUR_TOKEN.type == TokenType::KEYWORD)
    {
        Node* ret = GetReturn(storage, error);

        return ret;
    }

    if (CUR_TOKEN.type == TokenType::VAR)
    {
        Node* assign = GetAssignment(storage, error);

        return assign;
    }

    return nullptr;
}

// -------------------------------------------------------------

static Node* GetIfSection(SyntaxStorage* storage, error_t* error)
{
    assert(storage);
    assert(error);

    SKIP_BREAKS;

    Operators opt = TranslateKeywordToOperator(TOKEN_KEYWORD);

    SYN_ASSERT(opt == Operators::IF && CUR_TOKEN.type == TokenType::KEYWORD);

    Node* _if = MakeNode(NodeType::OP, {.opt = opt});
    INCREASE_PTR;

    SYN_ASSERT(CUR_TOKEN.type     == TokenType::OP &&
               CUR_TOKEN.info.opt == Operators::L_BRACKET)
    INCREASE_PTR;

    Node* cond = GetExpression(storage, error);


    SYN_ASSERT(CUR_TOKEN.type     == TokenType::OP &&
               CUR_TOKEN.info.opt == Operators::R_BRACKET);
    INCREASE_PTR;

    SKIP_BREAKS;

    Node* action = GetBlock(storage, error);


    ConnectNodes(_if, cond, action);

    opt = TranslateKeywordToOperator(TOKEN_KEYWORD);

    SYN_ASSERT(CUR_TOKEN.type == TokenType::KEYWORD &&
                          opt == Operators::CLOSE_BLOCK);
    INCREASE_PTR;

    Node* _break = GetBreak(storage, error);

    ConnectNodes(_break, _if, nullptr);

    return _break;
}

// -------------------------------------------------------------

static Node* GetWhileSection(SyntaxStorage* storage, error_t* error)  //TODO Parser state
{
    assert(storage);
    assert(error);

    SKIP_BREAKS;

    Operators opt = TranslateKeywordToOperator(TOKEN_KEYWORD);

    SYN_ASSERT(opt == Operators::WHILE && CUR_TOKEN.type == TokenType::KEYWORD);

    Node* _while = MakeNode(NodeType::OP, {.opt = opt});
    INCREASE_PTR;

    SYN_ASSERT(CUR_TOKEN.type     == TokenType::OP &&
               CUR_TOKEN.info.opt == Operators::L_BRACKET)      // TODO CONSUME
    INCREASE_PTR;

    Node* cond = GetExpression(storage, error);

    SYN_ASSERT(CUR_TOKEN.type     == TokenType::OP &&
               CUR_TOKEN.info.opt == Operators::R_BRACKET);
    INCREASE_PTR;

    SKIP_BREAKS;

    Node* action = GetBlock(storage, error);

    ConnectNodes(_while, cond, action);

    opt = TranslateKeywordToOperator(TOKEN_KEYWORD);

    SYN_ASSERT(CUR_TOKEN.type == TokenType::KEYWORD &&
                          opt == Operators::CLOSE_BLOCK);
    INCREASE_PTR;

    Node* _break = GetBreak(storage, error);
    ConnectNodes(_break, _while, nullptr);

    return _break;
}

// -------------------------------------------------------------

static Node* GetAssignment(SyntaxStorage* storage, error_t* error)
{
    assert(storage);
    assert(error);

    SKIP_BREAKS;

    /*SYN_ASSERT(CUR_TOKEN.type == TokenType::VAR);
    Node* var = MakeNode(NodeType::VAR, {.var = CUR_TOKEN.info.var});
    INCREASE_PTR;*/

    Node* var = GetVar(storage, error);
    if (error->code) return nullptr;

    // if (!error->code) return nullptr;

    Operators opt = TranslateKeywordToOperator(TOKEN_KEYWORD);

    SYN_ASSERT(CUR_TOKEN.type == TokenType::KEYWORD && opt == Operators::ASSIGN)
    INCREASE_PTR;

    Node* val = GetExpression(storage, error);

    Node* assign = MakeNode(NodeType::OP, {.opt = opt}, var, val);

    Node* _break = GetBreak(storage, error);

    ConnectNodes(_break, assign, nullptr);

    return _break;
}

// -------------------------------------------------------------

static Node* GetReturn(SyntaxStorage* storage, error_t* error)
{
    assert(storage);
    assert(error);

    SKIP_BREAKS;

    Operators opt = TranslateKeywordToOperator(TOKEN_KEYWORD);

    SYN_ASSERT(opt == Operators::RETURN && CUR_TOKEN.type == TokenType::KEYWORD);
    INCREASE_PTR;

    Node* expr = GetExpression(storage, error);

    Node* _return = MakeNode(NodeType::OP, {.opt = opt}, expr);

    Node* _break = GetBreak(storage, error);

    ConnectNodes(_break, _return, nullptr);

    return _break;
}

// -------------------------------------------------------------

static Node* GetExpression(SyntaxStorage* storage, error_t* error)
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


    Node* val = GetComparison(storage, error);


    Operators opt = TranslateKeywordToOperator(TOKEN_KEYWORD);

    if (CUR_TOKEN.type == TokenType::KEYWORD &&
       (opt == Operators::LESS    || opt == Operators::LESSEQUAL    || opt == Operators::EQUAL ||
        opt == Operators::GREATER || opt == Operators::GREATEREQUAL || opt == Operators::NOT_EQUAL))
    {
        Node* op = MakeNode(NodeType::OP, {.opt = opt});
        INCREASE_PTR;


        Node* val2 = GetComparison(storage, error);


        val = ConnectNodes(op, val, val2);

        opt = TranslateKeywordToOperator(TOKEN_KEYWORD);
    }

    return val;
}

// -------------------------------------------------------------

static Node* GetComparison(SyntaxStorage* storage, error_t* error)
{
    assert(storage);
    assert(error);


    Node* val = GetTerm(storage, error);


    while (CUR_TOKEN.type     == TokenType::OP  &&
          (CUR_TOKEN.info.opt == Operators::ADD ||
           CUR_TOKEN.info.opt == Operators::SUB))
    {
        Node* op = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
        INCREASE_PTR;


        Node* val2 = GetTerm(storage, error);


        val = ConnectNodes(op, val, val2);
    }
    return val;
}

// -------------------------------------------------------------

static Node* GetTerm(SyntaxStorage* storage, error_t* error)
{
    assert(error);
    assert(storage);


    Node* val = GetDegree(storage, error);


    while (CUR_TOKEN.type     == TokenType::OP  &&
          (CUR_TOKEN.info.opt == Operators::DIV ||
           CUR_TOKEN.info.opt == Operators::MUL))
    {
        Node* op = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
        INCREASE_PTR;


        Node* val2 = GetDegree(storage, error);


        val = ConnectNodes(op, val, val2);
    }
    return val;
}

// -------------------------------------------------------------

static Node* GetDegree(SyntaxStorage* storage, error_t* error)
{
    assert(error);
    assert(storage);


    Node* val = GetTrigonometry(storage, error);


    while (CUR_TOKEN.type     == TokenType::OP &&
           CUR_TOKEN.info.opt == Operators::DEG)
    {
        Node* op = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
        INCREASE_PTR;


        Node* val2 = GetTrigonometry(storage, error);


        val = ConnectNodes(op, val, val2);
    }
    return val;
}

// -------------------------------------------------------------

static Node* GetTrigonometry(SyntaxStorage* storage, error_t* error)
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

        Node* val = GetBrackets(storage, error);


        val = ConnectNodes(op, val, nullptr);

        return val;
    }

    return GetBrackets(storage, error);

}

// -------------------------------------------------------------

static Node* GetBrackets(SyntaxStorage* storage, error_t* error)
{
    assert(error);
    assert(storage);


    if (CUR_TOKEN.type     == TokenType::OP &&
        CUR_TOKEN.info.opt == Operators::L_BRACKET)
    {
        INCREASE_PTR;
        Node* val = GetExpression(storage, error);


        SYN_ASSERT(CUR_TOKEN.type     == TokenType::OP &&
                   CUR_TOKEN.info.opt == Operators::R_BRACKET);
        INCREASE_PTR;

        return val;
    }

    return GetNumber(storage, error);

}

// -------------------------------------------------------------

static Node* GetNumber(SyntaxStorage* storage, error_t* error)
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

        Node* num = nullptr;

        if (CUR_TOKEN.type == TokenType::NUM)
            num = MakeNode(NodeType::NUM, {.val = CUR_TOKEN.info.val});
        else
            num = MakeNode(NodeType::VAR, {.val = CUR_TOKEN.info.var});

        INCREASE_PTR;

        val = ConnectNodes(op, MakeNode(NodeType::NUM, {.val = 0}), num);
    }
    else
    {
        SYN_ASSERT(CUR_TOKEN.type == TokenType::NUM || CUR_TOKEN.type == TokenType::VAR);

        Node* num = nullptr;

        if (CUR_TOKEN.type == TokenType::NUM)
            num = MakeNode(NodeType::NUM, {.val = CUR_TOKEN.info.val});
        else
            num = MakeNode(NodeType::VAR, {.val = CUR_TOKEN.info.var});

        INCREASE_PTR;

        val = num;
    }

    return val;
}

