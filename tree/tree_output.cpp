#include <stdio.h>

#include "tree.h"
#include "visual.h"
#include "tree_output.h"

static void PrintNodeDataType(FILE* fp, const NodeType type);

// ======================================================================
// GRAPH BUILDING
// ======================================================================

static void        DrawTreeGraph(const tree_t* tree);

static inline void DrawNodes(FILE* dotf, const tree_t* tree, const Node* node, const int rank);
static inline void FillNodeColor(FILE* fp, const Node* node);


//-----------------------------------------------------------------------------------------------------

int NodeDump(FILE* fp, const void* dumping_node, const char* func, const char* file, const int line)
{
    assert(dumping_node);

    LOG_START_DUMP(func, file, line);

    const Node* node = (const Node*) dumping_node;

    fprintf(fp, "NODE [%p]<br>\n"
                "LEFT > [%p]<br>\n"
                "RIGHT > [%p]<br>\n"
                "TYPE > ", node, node->left, node->right);

    PrintNodeDataType(fp, node->type);

    fprintf(fp, "<br>\n"
                "VALUE > %d<br>\n", node->value);

    LOG_END();

    return (int) TreeErrors::NONE;
}

//-----------------------------------------------------------------------------------------------------

#ifdef CHECK_SHORT_OP
#undef CHECK_SHORT_OP
#endif
#define CHECK_SHORT_OP(opt)             \
    case (Operators::opt):        \
        fprintf(fp, "%c", opt);   \
        break;                    \

#ifdef CHECK_LONG_OP
#undef CHECK_LONG_OP
#endif
#define CHECK_LONG_OP(opt)        \
    case (Operators::opt):        \
        fprintf(fp, #opt);   \
        break;                    \

void PrintOperator(FILE* fp, const Operators sign)
{
    switch (sign)
    {

        CHECK_SHORT_OP(ADD);
        CHECK_SHORT_OP(SUB);
        CHECK_SHORT_OP(MUL);
        CHECK_SHORT_OP(DIV);
        CHECK_SHORT_OP(DEG);
        CHECK_SHORT_OP(COMMA);
        CHECK_SHORT_OP(L_BRACKET);
        CHECK_SHORT_OP(R_BRACKET);

        CHECK_LONG_OP(BREAK);
        CHECK_LONG_OP(SIN);
        CHECK_LONG_OP(COS);
        CHECK_LONG_OP(ASSIGN);
        CHECK_LONG_OP(IF);
        CHECK_LONG_OP(WHILE);
        CHECK_LONG_OP(AND);
        CHECK_LONG_OP(OR);
        CHECK_LONG_OP(GREATER);
        CHECK_LONG_OP(GREATEREQUAL);
        CHECK_LONG_OP(LESSEQUAL);
        CHECK_LONG_OP(LESS);
        CHECK_LONG_OP(EQUAL);
        CHECK_LONG_OP(NOT_EQUAL);
        CHECK_LONG_OP(RETURN);
        CHECK_LONG_OP(ELSE);
        CHECK_LONG_OP(OPEN_BLOCK);
        CHECK_LONG_OP(CLOSE_BLOCK);

        default:
            fprintf(fp, " undefined_operator ");
    }
}

#undef CHECK_SHORT_OP

//-----------------------------------------------------------------------------------------------------

static void PrintNodeDataType(FILE* fp, const NodeType type)
{
    switch (type)
    {
        case (NodeType::NUM):
            fprintf(fp, "number");
            break;
        case (NodeType::OP):
            fprintf(fp, "operator");
            break;
        case (NodeType::VAR):
            fprintf(fp, "variable");
            break;
        default:
            fprintf(fp, "unknown_type");
    }
}

//-----------------------------------------------------------------------------------------------------

void PrintNodeData(FILE* fp, const tree_t* tree, const Node* node)
{
    if (!node)
        return;

    switch(node->type)
    {
        case (NodeType::NUM):
            fprintf(fp, "%d", node->value.val);
            break;
        case (NodeType::VAR):
            fprintf(fp, "[%d]", node->value.var);
            break;
        case (NodeType::OP):
            PrintOperator(fp, node->value.opt);
            break;
        default:
            fprintf(fp, " undefined ");
    }
}

//-----------------------------------------------------------------------------------------------------

int TreeDump(FILE* fp, const void* nodes, const char* func, const char* file, const int line)
{
    assert(nodes);

    LOG_START_DUMP(func, file, line);

    const tree_t* tree = (const tree_t*) nodes;

    DrawTreeGraph(tree);

    LOG_END();

    return (int) TreeErrors::NONE;
}

//-----------------------------------------------------------------------------------------------------

static void TextTreeDump(FILE* fp, const tree_t* tree)
{
    assert(tree);

    fprintf(fp, "<pre>");

    fprintf(fp, "<b>DUMPING EXPRESSION</b>\n");

    // PrintInfixTree(fp, tree);

    fprintf(fp, "</pre>");
}

//-----------------------------------------------------------------------------------------------------

static void DrawTreeGraph(const tree_t* tree)
{
    assert(tree);

    FILE* dotf = fopen(TMP_DOT_FILE, "w");
    if (dotf == nullptr)
    {
        PrintLog("CAN NOT DRAW TREE GRAPH<br>\n");
        return;
    }

    StartGraph(dotf);
    DrawNodes(dotf, tree, tree->root, 1);
    EndGraph(dotf);

    fclose(dotf);

    MakeImgFromDot(TMP_DOT_FILE);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::;::::::::::::::::::::::::::

static inline void DrawNodes(FILE* dotf, const tree_t* tree, const Node* node, const int rank)
{
    if (!node) return;

    fprintf(dotf, "%lld [shape=Mrecord, style=filled, " , node);

    FillNodeColor(dotf, node);

    fprintf(dotf, " rank = %d, label=\" "
                  "{ node: %p | parent: %p | { type: " ,rank, node, node->parent);

    PrintNodeDataType(dotf, node->type);

    fprintf(dotf, " | data: ");

    PrintNodeData(dotf, tree, node);

    fprintf(dotf, "} | { left: %p| right: %p } }\"]\n", node->left, node->right);

    DrawNodes(dotf, tree, node->left, rank + 1);
    DrawNodes(dotf, tree, node->right, rank + 1);

    if (node->parent != nullptr)
        fprintf(dotf, "%lld->%lld [color = blue]\n", node, node->parent);

    if (node->left != nullptr)
        fprintf(dotf, "%lld->%lld [color = black, fontcolor = black]\n", node, node->left);

    if (node->right != nullptr)
        fprintf(dotf, "%lld->%lld [color = black, fontcolor = black]\n", node, node->right);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::;::::::::::::::::::::::::::

static inline void FillNodeColor(FILE* fp, const Node* node)
{
    assert(node);

    switch (node->type)
    {
        case (NodeType::NUM):
            fprintf(fp, "fillcolor = \"lightblue\", color = \"darkblue\",");
            break;
        case (NodeType::VAR):
            fprintf(fp, "fillcolor = \"lightgreen\", color = \"darkgreen\",");
            break;
        case (NodeType::OP):
            fprintf(fp, "fillcolor = \"yellow\", color = \"goldenrod\",");
            break;
        case (NodeType::POISON):
        // break through
        default:
            fprintf(fp, "fillcolor = \"lightgray\", color = \"darkgray\",");
    }
}


