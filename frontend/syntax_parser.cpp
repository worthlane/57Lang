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
                                        storage->ptr++,                __LINE__)

#ifdef  DECREASE_PTR
#undef  DECREASE_PTR
#endif
#define DECREASE_PTR    PrintLog("RETURN FROM TOKEN[%d], READING PREVIOUS ON LINE [%d]<br>\n",       \
                                                    storage->ptr++,                __LINE__)

#ifdef  CUR_TOKEN
#undef  CUR_TOKEN
#endif
#define CUR_TOKEN       storage->lexis->tokens[CUR_PTR]

#ifdef  SKIP_BREAKS
#undef  SKIP_BREAKS
#endif
#define SKIP_BREAKS     do                                                                                      \
                        {                                                                                       \
                            while(CUR_TOKEN.info.opt == Operators::BREAK && CUR_TOKEN.type == NodeType::OP)       \
                            {                                                   \
                                INCREASE_PTR;                                   \
                            }                                                   \
                        } while (false);

#ifdef  SYN_ASSERT
#undef  SYN_ASSERT
#endif
#define SYN_ASSERT(stat)                                                                            \
        do                                                                                              \
        {                                                                                               \
            if (!(stat))                                                                                \
            {                                                                                           \
                LOG_START(__func__, __FILE__, __LINE__);                                                \
                error->code = (int) FrontendErrors::INVALID_SYNTAX;                                     \
                SetErrorData(error, "SYNTAX ASSERT\"" #stat "\"<br>\n"                                  \
                                    "IN FUNCTION %s FROM FILE \"%s\"(%d)<br>\n",                        \
                                    __func__, __FILE__, __LINE__);                                      \
                LOG_END();                                                                              \
                return nullptr;                                                                         \
            }                                                                                           \
        } while (false)                                                                                 \


#ifdef  CONSUME
#undef  CONSUME
#endif
#define CONSUME(stat)                           \
        do                                      \
        {                                       \
            SYN_ASSERT(stat);                   \
            INCREASE_PTR;                       \
        } while (false)


#ifdef  PEEK
#undef  PEEK
#endif
#define PEEK(op, type)   CUR_TOKEN.info.opt == op && CUR_TOKEN.type == type

#ifdef  BREAK_ASSERT
#undef  BREAK_ASSERT
#endif
#define BREAK_ASSERT(stat)                                                                            \
        do                                                                                          \
        {                                                                                           \
            if (!(stat))                                                                                \
            {                                                                                           \
                LOG_START(__func__, __FILE__, __LINE__);                                                \
                PrintLog("LEAVING FUNC BECAUSE OF\"" #stat "\"<br>\n"                                  \
                        "IN FUNCTION %s, FILE \"%s\"(%d)<br>\n",                                         \
                                    __func__, __FILE__, __LINE__);                                      \
                LOG_END();                                                                              \
                return nullptr;                                                                         \
            }                                                                                           \
        } while (false)

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
    CONSUME(CUR_TOKEN.type     == NodeType::OP &&
            CUR_TOKEN.info.opt == Operators::BREAK);

    Node* break_token = MakeNode(NodeType::OP, {.opt = Operators::BREAK});

    return break_token;
}

// -------------------------------------------------------------

static Node* GetType(SyntaxStorage* storage, error_t* error)
{
    SYN_ASSERT(IsType(CUR_TOKEN.info.opt) && CUR_TOKEN.type == NodeType::OP);
    Node* type = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
    INCREASE_PTR;

    return type;
}

// -------------------------------------------------------------

static Node* GetVar(SyntaxStorage* storage, error_t* error)
{
    assert(storage);
    assert(error);

    SYN_ASSERT(CUR_TOKEN.type == NodeType::VAR);

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

    SYN_ASSERT(CUR_TOKEN.info.opt == Operators::END && CUR_TOKEN.type == NodeType::OP);

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

    // TODO if (peek(storagr, NodeType::IF))
    // TODO if (PEEK(NodeType::IF))
    // TODO if (storage->peek(NodeType::IF))
    if (CUR_TOKEN.info.opt == Operators::IF && CUR_TOKEN.type == NodeType::OP)
    {
        Node* if_token = GetIfSection(storage, error);

        return if_token;
    }

    if (CUR_TOKEN.info.opt == Operators::WHILE && CUR_TOKEN.type == NodeType::OP)
    {
        Node* while_token = GetWhileSection(storage, error);

        return while_token;
    }

    if (CUR_TOKEN.info.opt == Operators::RETURN && CUR_TOKEN.type == NodeType::OP)
    {
        Node* ret = GetReturn(storage, error);

        return ret;
    }

    if (CUR_TOKEN.type == NodeType::VAR)
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

    SYN_ASSERT(CUR_TOKEN.info.opt == Operators::IF && CUR_TOKEN.type == NodeType::OP);
    Node* if_token = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
    INCREASE_PTR;

    CONSUME(CUR_TOKEN.type     == NodeType::OP &&
            CUR_TOKEN.info.opt == Operators::L_BRACKET);

    Node* cond = GetExpression(storage, error);


    CONSUME(CUR_TOKEN.type     == NodeType::OP &&
            CUR_TOKEN.info.opt == Operators::R_BRACKET);

    SKIP_BREAKS;

    Node* action = GetBlock(storage, error);

    ConnectNodes(if_token, cond, action);

    CONSUME(CUR_TOKEN.type     == NodeType::OP &&
            CUR_TOKEN.info.opt == Operators::CLOSE_BLOCK);

    Node* break_token = GetBreak(storage, error);

    ConnectNodes(break_token, if_token, nullptr);

    return break_token;
}

// -------------------------------------------------------------

static Node* GetWhileSection(SyntaxStorage* storage, error_t* error)  //TODO Parser state
{
    assert(storage);
    assert(error);

    SKIP_BREAKS;

    SYN_ASSERT(CUR_TOKEN.info.opt == Operators::WHILE && CUR_TOKEN.type == NodeType::OP);
    Node* while_token = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
    INCREASE_PTR;

    CONSUME(CUR_TOKEN.type     == NodeType::OP &&
            CUR_TOKEN.info.opt == Operators::L_BRACKET);

    Node* cond = GetExpression(storage, error);

    CONSUME(CUR_TOKEN.type     == NodeType::OP &&
            CUR_TOKEN.info.opt == Operators::R_BRACKET);

    SKIP_BREAKS;

    Node* action = GetBlock(storage, error);

    ConnectNodes(while_token, cond, action);

    CONSUME(CUR_TOKEN.type     == NodeType::OP &&
            CUR_TOKEN.info.opt == Operators::CLOSE_BLOCK);

    Node* break_token = GetBreak(storage, error);
    ConnectNodes(break_token, while_token, nullptr);

    return break_token;
}

// -------------------------------------------------------------

static Node* GetAssignment(SyntaxStorage* storage, error_t* error)
{
    assert(storage);
    assert(error);

    SKIP_BREAKS;

    Node* var = GetVar(storage, error);
    if (error->code) return nullptr;

    SYN_ASSERT(CUR_TOKEN.type == NodeType::OP && CUR_TOKEN.info.opt == Operators::ASSIGN);
    Node* assign = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
    INCREASE_PTR;

    Node* val    = GetExpression(storage, error);
    Node* break_token = GetBreak(storage, error);

    ConnectNodes(assign, var, val);

    ConnectNodes(break_token, assign, nullptr);

    return break_token;
}

// -------------------------------------------------------------

static Node* GetReturn(SyntaxStorage* storage, error_t* error)
{
    assert(storage);
    assert(error);

    SKIP_BREAKS;

    SYN_ASSERT(CUR_TOKEN.info.opt == Operators::RETURN && CUR_TOKEN.type == NodeType::OP);
    Node* ret = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
    INCREASE_PTR;

    Node* expr   = GetExpression(storage, error);
    Node* break_token = GetBreak(storage, error);

    ConnectNodes(ret, expr, nullptr);
    ConnectNodes(break_token, ret, nullptr);

    return break_token;
}

// -------------------------------------------------------------

static Node* GetExpression(SyntaxStorage* storage, error_t* error)
{
    assert(error);
    assert(storage);

    Node* val = GetOrOperand(storage, error);

    while (CUR_TOKEN.type == NodeType::OP && CUR_TOKEN.info.opt == Operators::OR)
    {
        Node* op = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
        INCREASE_PTR;

        Node* val2 = GetOrOperand(storage, error);

        val = ConnectNodes(op, val, val2);
    }

    return val;
}


// -------------------------------------------------------------

static Node* GetOrOperand(SyntaxStorage* storage, error_t* error)
{
    assert(error);
    assert(storage);

    Node* val = GetAndOperand(storage, error);

    while (CUR_TOKEN.type == NodeType::OP && CUR_TOKEN.info.opt == Operators::AND)
    {
        Node* op = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
        INCREASE_PTR;

        Node* val2 = GetAndOperand(storage, error);

        val = ConnectNodes(op, val, val2);
    }

    return val;
}

// -------------------------------------------------------------

static Node* GetAndOperand(SyntaxStorage* storage, error_t* error)
{
    assert(error);
    assert(storage);

    Node* val = GetComparison(storage, error);

    if (CUR_TOKEN.type == NodeType::OP &&
       (CUR_TOKEN.info.opt == Operators::LESS         || CUR_TOKEN.info.opt == Operators::LESSEQUAL ||
        CUR_TOKEN.info.opt == Operators::EQUAL        || CUR_TOKEN.info.opt == Operators::GREATER   ||
        CUR_TOKEN.info.opt == Operators::GREATEREQUAL || CUR_TOKEN.info.opt == Operators::NOT_EQUAL))
    {
        Node* op = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
        INCREASE_PTR;

        Node* val2 = GetComparison(storage, error);

        val = ConnectNodes(op, val, val2);
    }

    return val;
}

// -------------------------------------------------------------

static Node* GetComparison(SyntaxStorage* storage, error_t* error)
{
    assert(storage);
    assert(error);

    Node* val = GetTerm(storage, error);

    while (CUR_TOKEN.type     == NodeType::OP  &&
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

    while (CUR_TOKEN.type     == NodeType::OP  &&
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

    while (CUR_TOKEN.type     == NodeType::OP &&
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

    if (CUR_TOKEN.type     == NodeType::OP &&
       (CUR_TOKEN.info.opt == Operators::SIN    ||
        CUR_TOKEN.info.opt == Operators::COS))
    {
        Node* op = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
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

    if (CUR_TOKEN.type     == NodeType::OP &&
        CUR_TOKEN.info.opt == Operators::L_BRACKET)
    {
        INCREASE_PTR;
        Node* val = GetExpression(storage, error);

        SYN_ASSERT(CUR_TOKEN.type     == NodeType::OP &&
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

    if  (CUR_TOKEN.type     == NodeType::OP  &&
        (CUR_TOKEN.info.opt == Operators::ADD ||
         CUR_TOKEN.info.opt == Operators::SUB))
    {
        Node* op = MakeNode(NodeType::OP, {.opt = CUR_TOKEN.info.opt});
        INCREASE_PTR;

        SYN_ASSERT(CUR_TOKEN.type == NodeType::NUM || CUR_TOKEN.type == NodeType::VAR);

        Node* num = nullptr;

        if (CUR_TOKEN.type == NodeType::NUM)
            num = MakeNode(NodeType::NUM, {.val = CUR_TOKEN.info.val});
        else
            num = MakeNode(NodeType::VAR, {.val = CUR_TOKEN.info.var});

        INCREASE_PTR;

        val = ConnectNodes(op, MakeNode(NodeType::NUM, {.val = 0}), num);
    }
    else
    {
        SYN_ASSERT(CUR_TOKEN.type == NodeType::NUM || CUR_TOKEN.type == NodeType::VAR);

        Node* num = nullptr;

        if (CUR_TOKEN.type == NodeType::NUM)
            num = MakeNode(NodeType::NUM, {.val = CUR_TOKEN.info.val});
        else
            num = MakeNode(NodeType::VAR, {.val = CUR_TOKEN.info.var});

        INCREASE_PTR;

        val = num;
    }

    return val;
}

