#ifndef __EXPRESSION_H_
#define __EXPRESSION_H_

#include <stdio.h>

#include "common/errors.h"
#include "common/file_read.h"

// ======================================================================
// ERRORS
// ======================================================================

enum class TreeErrors
{
    NONE = 0,

    ALLOCATE_MEMORY,
    NO_EXPRESSION,
    INVALID_SYNTAX,
    CYCLED_NODE,
    COMMON_HEIR,
    INVALID_EXPRESSION_FORMAT,
    UNKNOWN_OPERATION,
    NO_DIFF_VARIABLE,

    UNKNOWN
};
int PrintTreeError(FILE* fp, const void* err, const char* func, const char* file, const int line);

#ifdef EXIT_IF_EXPRESSION_ERROR
#undef EXIT_IF_EXPRESSION_ERROR
#endif
#define EXIT_IF_EXPRESSION_ERROR(error)     do                                                            \
                                            {                                                           \
                                                if ((error)->code != (int) TreeErrors::NONE)      \
                                                {                                                       \
                                                    return LogDump(PrintTreeError, error, __func__,     \
                                                                    __FILE__, __LINE__);                \
                                                }                                                       \
                                            } while(0)
#ifdef RETURN_IF_EXPRESSION_ERROR
#undef RETURN_IF_EXPRESSION_ERROR
#endif
#define RETURN_IF_EXPRESSION_ERROR(error)   do                                                            \
                                            {                                                           \
                                                if ((error) != TreeErrors::NONE)                  \
                                                {                                                       \
                                                    return error;                                       \
                                                }                                                       \
                                            } while(0)

// ======================================================================
// OPERATORS
// ======================================================================

#define DEF_OP(name, ...)      name,

enum class Operators
{
    ADD,
    SUB,
    MUL,
    DIV,

    ASSIGN,

    UNK
};

#undef DEF_OP

// ======================================================================
// NODES
// ======================================================================

enum class NodeType
{
    VAR,
    OP,
    NUM,

    POISON,
};

union NodeValue
{
    int         val;
    Operators   opt;
    int         var;
};
static const NodeValue ZERO_VALUE = {.val = 0};

enum class NodeKid
{
    RIGHT,
    LEFT
};

struct Node
{
    Node* parent;

    NodeType  type;
    NodeValue value;

    Node* left;
    Node* right;
};

Node* MakeNode(const NodeType type, const NodeValue value,
               Node* left = nullptr, Node* right = nullptr, Node* parent = nullptr);
void  NodeDtor(Node* node);
void  DestructNodes(Node* root);
void  FillNode(Node* node, Node* left, Node* right, Node* parent, const NodeType type, const NodeValue value);
Node* ConnectNodes(Node* node, Node* left, Node* right);

TreeErrors NodeVerify(const Node* node, error_t* error);

#ifdef CHECK_NODE
#undef CHECK_NODE
#endif
#define CHECK_NODE(node, error)     do                                                              \
                                    {                                                               \
                                        DiffErrors node_err_ = NodeVerify(node, error);             \
                                        if (node_err_ != TreeErrors::NONE)                          \
                                            return node_err_;                                       \
                                    } while(0)

void LinkNodesWithParents(Node* node);

// ======================================================================
// EXPRESSION STRUCT
// ======================================================================

struct tree_t
{
    Node* root;
};

TreeErrors          TreeCtor(tree_t* tree, error_t* error);
tree_t*             MakeTree(error_t* error);
void                TreeDtor(tree_t* tree);

TreeErrors          TreeVerify(const tree_t* tree, error_t* error);

#endif


