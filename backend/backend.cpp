#include <stdio.h>
#include <string.h>

#include "backend.h"
#include "stack/stack.h"

#ifdef  NODE_NAME
#undef  NODE_NAME
#endif
#define NODE_NAME(token) (tree->names.list[token->value.var].name)

#ifdef  LOCAL_TABLE
#undef  LOCAL_TABLE
#endif
#define LOCAL_TABLE(stk) (stk->data[stk->size - 1])

static const int NO_RAM = -1;

static int GetNameRamIdFromStack(const Stack_t* stk, const char* name);
static int GetNameRamIdFromTable(const nametable_t* nametable, const char* name);

static void TranslateToAsm(const tree_t* tree, const Node* node, Stack_t* tables, FILE* out_stream, error_t* error);

static void TranslateCompareToAsm(tree_t* tree, const Node* node, Stack_t* tables, int* label_spot,
                                  const char* comparator, FILE* out_stream, error_t* error);

static void TranslateAndToAsm(tree_t* tree, const Node* node, Stack_t* tables,
                              int* label_spot, FILE* out_stream, error_t* error);
static void TranslateOrToAsm(tree_t* tree, const Node* node, Stack_t* tables,
                              int* label_spot, FILE* out_stream, error_t* error);

static void TranslateAssignToAsm(tree_t* tree, Node* node, Stack_t* tables,
                                 int* ram_spot, FILE* out_stream, error_t* error);

//-----------------------------------------------------------------------------------------------------

void TranslateToAsm(const tree_t* tree, FILE* out_stream, error_t* error)
{
    assert(tree);

    Stack_t tables = {};
    StackCtor(&tables);

    nametable_t* global = MakeNametable();
    StackPush(&tables, global);

    TranslateToAsm(tree, tree->root, &tables, out_stream, error);

    fprintf(out_stream, "hlt\n");

    StackDtor(&tables);
}

//-----------------------------------------------------------------------------------------------------

static void TranslateToAsm(tree_t* tree, const Node* node, Stack_t* tables, FILE* out_stream, error_t* error)
{
    assert(tree);

    static int ram_spot   = 0;
    static int label_spot = 0;

    if (node == nullptr)
        return;

    if (node->type == NodeType::NUM)
    {
        fprintf(out_stream, "push %d\n", node->value.val);
        return;
    }

    if (node->type == NodeType::VAR)
    {
        int ram_id = GetNameRamIdFromStack(tables, NODE_NAME(node));
        if (ram_id == NO_RAM)
            error->code = (int) BackendErrors::NON_INIT_VAR;

        fprintf(out_stream, "push [%d]\n", ram_id);
        return;
    }

    assert(node->type == NodeType::OP);

    switch (node->value.opt)
    {
        case (Operators::ADD):
        {
            TranslateToAsm(tree, node->left, tables, out_stream, error);
            TranslateToAsm(tree, node->right, tables, out_stream, error);
            fprintf(out_stream, "add\n");
            break;
        }
        case (Operators::SUB):
        {
            TranslateToAsm(tree, node->left, tables, out_stream, error);
            TranslateToAsm(tree, node->right, tables, out_stream, error);
            fprintf(out_stream, "sub\n");
            break;
        }
        case (Operators::MUL):
        {
            TranslateToAsm(tree, node->left, tables, out_stream, error);
            TranslateToAsm(tree, node->right, tables, out_stream, error);
            fprintf(out_stream, "mul\n");
            break;
        }
        case (Operators::DIV):
        {
            TranslateToAsm(tree, node->left, tables, out_stream, error);
            TranslateToAsm(tree, node->right, tables, out_stream, error);
            fprintf(out_stream, "div\n");
            break;
        }
        case (Operators::DEG):
        {
            TranslateToAsm(tree, node->left, tables, out_stream, error);
            TranslateToAsm(tree, node->right, tables, out_stream, error);
            fprintf(out_stream, "pow\n");
            break;
        }
        case (Operators::SIN):
        {
            TranslateToAsm(tree, node->left, tables, out_stream, error);
            fprintf(out_stream, "sin\n");
            break;
        }
        case (Operators::COS):
        {
            TranslateToAsm(tree, node->left, tables, out_stream, error);
            fprintf(out_stream, "cos\n");
            break;
        }
        case (Operators::INPUT):
        {
            fprintf(out_stream, "in\n");
            break;
        }
        case (Operators::OUTPUT):
        {
            TranslateToAsm(tree, node->left, tables, out_stream, error);
            fprintf(out_stream, "out\n");
            break;
        }
        case (Operators::COMMA):
        {
            TranslateToAsm(tree, node->right, tables, out_stream, error);
            TranslateToAsm(tree, node->left, tables, out_stream, error);
            break;
        }
        case (Operators::BREAK):
        {
            TranslateToAsm(tree, node->left, tables, out_stream, error);
            TranslateToAsm(tree, node->right, tables, out_stream, error);
            break;
        }
        case (Operators::NEW_FUNC):
        {
            TranslateToAsm(tree, node->right, tables, out_stream, error);
            TranslateToAsm(tree, node->left, tables, out_stream, error);
            break;
        }
        case (Operators::GREATER):
        {
            TranslateCompareToAsm(tree, node, tables, &label_spot, "jb", out_stream, error);
            break;
        }
        case (Operators::GREATEREQUAL):
        {
            TranslateCompareToAsm(tree, node, tables, &label_spot, "jbe", out_stream, error);
            break;
        }
        case (Operators::LESS):
        {
            TranslateCompareToAsm(tree, node, tables, &label_spot, "ja", out_stream, error);
            break;
        }
        case (Operators::LESSEQUAL):
        {
            TranslateCompareToAsm(tree, node, tables, &label_spot, "jae", out_stream, error);
            break;
        }
        case (Operators::EQUAL):
        {
            TranslateCompareToAsm(tree, node, tables, &label_spot, "je", out_stream, error);
            break;
        }
        case (Operators::NOT_EQUAL):
        {
            TranslateCompareToAsm(tree, node, tables, &label_spot, "jne", out_stream, error);
            break;
        }
        case (Operators::AND):
        {
            TranslateAndToAsm(tree, node, tables, &label_spot, out_stream, error);
            break;
        }
        case (Operators::OR):
        {
            TranslateOrToAsm(tree, node, tables, &label_spot, out_stream, error);
            break;
        }
        case (Operators::TYPE):
        {
            TranslateToAsm(tree, node->right, tables, out_stream, error);
            break;
        }
        case (Operators::RETURN):
        {
            TranslateToAsm(tree, node->left, tables, out_stream, error);
            fprintf(out_stream, "ret\n");
            break;
        }
        case (Operators::ASSIGN):
        {
            TranslateAssignToAsm(tree, node->left, tables, &ram_spot, out_stream, error);
            break;
        }
        case (Operators::IF):
        {
            // TranslateIfToAsm(tree, node->left, tables, &label_spot, out_stream, error);
            break;
        }
        case (Operators::WHILE):
        {
            // TranslateWhileToAsm(tree, node->left, tables, &label_spot, out_stream, error);
            break;
        }

        case (Operators::FUNC):
        case (Operators::FUNC_CALL):
    }

}

//-----------------------------------------------------------------------------------------------------

static int GetNameRamIdFromTable(const nametable_t* nametable, const char* name)
{
    assert(nametable);
    assert(name);

    for (int i = 0; i < nametable->size; i++)
    {
        if (!strncmp(name, nametable->list[i].name, MAX_NAME_LEN))
        {
            return nametable->list[i].ram_id;
        }
    }

    return NO_RAM;
}

//-----------------------------------------------------------------------------------------------------

static int GetNameRamIdFromStack(const Stack_t* stk, const char* name)
{
    assert(stk);
    assert(name);

    for (int i = 0; i < stk->size; i++)
    {
        int id = GetNameRamIdFromTable(stk->data[i], name);
        if (id != NO_RAM)
            return id;
    }

    return NO_RAM;
}

//-----------------------------------------------------------------------------------------------------

static void TranslateCompareToAsm(tree_t* tree, const Node* node, Stack_t* tables, int* label_spot,
                                  const char* comparator, FILE* out_stream, error_t* error)
{
    assert(tables);
    assert(tree);
    assert(node);
    assert(label_spot);
    assert(comparator);

    TranslateToAsm(tree, node->left, tables, out_stream, error);
    TranslateToAsm(tree, node->right, tables, out_stream, error);

    fprintf(out_stream, "%s :TRUE_%d         \% COMPARISON\n"
                        "push 0\n"
                        "jmp :FALSE_%d\n"
                        ":TRUE_%d\n"
                        "push 1\n"
                        ":FALSE_%d           \% COMPARISON END\n",
                        comparator, label_spot, label_spot, label_spot, label_spot);


    *label_spot++;
}

//-----------------------------------------------------------------------------------------------------

static void TranslateAndToAsm(tree_t* tree, const Node* node, Stack_t* tables,
                              int* label_spot, FILE* out_stream, error_t* error)
{
    assert(tables);
    assert(tree);
    assert(node);
    assert(label_spot);

    TranslateToAsm(tree, node->left, tables, out_stream, error);
    TranslateToAsm(tree, node->right, tables, out_stream, error);

    fprintf(out_stream, "mul                \% AND OPERATION\n"
                        "push 0\n"
                        "je :AND_TRUE_%d\n"
                        "push 1\n"
                        "jmp :AND_FALSE_%d\n"
                        ":AND_TRUE_%d\n"
                        "push 0\n"
                        ":AND_FALSE_%d      \% AND OPERATION END\n",
                        label_spot, label_spot, label_spot, label_spot);


    *label_spot++;
}

//-----------------------------------------------------------------------------------------------------

static void TranslateOrToAsm(tree_t* tree, const Node* node, Stack_t* tables,
                              int* label_spot, FILE* out_stream, error_t* error)
{
    assert(tables);
    assert(tree);
    assert(node);
    assert(label_spot);

    TranslateToAsm(tree, node->left, tables, out_stream, error);

    fprintf(out_stream, "push 0                \% FIRST OR OPERAND\n"
                        "je :FIRST_ZERO_%d\n"
                        "push 1\n"
                        "jmp :FIRST_NOT_ZERO_%d\n"
                        ":FIRST_ZERO_%d\n"
                        "push 0\n"
                        ":FIRST_NOT_ZERO_%d    \% END OF FIRST OR OPERAND\n",
                         *label_spot, *label_spot, *label_spot, *label_spot);

    *label_spot++;

    TranslateToAsm(tree, node->right, tables, out_stream, error);

    fprintf(out_stream, "push 0\n"
                        "je :SECOND_ZERO_%d    \% SECOND OR OPERAND\n"
                        "push 1\n"
                        "jmp :SECOND_NOT_ZERO_%d\n"
                        ":SECOND_ZERO_%d\n"
                        "push 0\n"
                        ":SECOND_NOT_ZERO_%d   \% END OF SECOND OR OPERAND\n",
                         *label_spot, *label_spot, *label_spot, *label_spot);

    *label_spot++;

    fprintf(out_stream, "add                   \% OR OPERATION\n"
                        "push 0\n"
                        "je :OR_TRUE_%d\n"
                        "push 1\n"
                        "jmp :OR_FALSE_%d\n"
                        ":OR_TRUE_%d\n"
                        "push 0\n"
                        ":OR_FALSE_%d           \% END OF OR OPERATION\n",
                         *label_spot, *label_spot, *label_spot, *label_spot);

    *label_spot++;

}

//-----------------------------------------------------------------------------------------------------

static void TranslateAssignToAsm(tree_t* tree, Node* node, Stack_t* tables,
                                 int* ram_spot, FILE* out_stream, error_t* error)
{
    assert(ram_spot);
    assert(tree);
    assert(node);
    assert(tables);
    assert(error);

    TranslateToAsm(tree, node->right, tables, out_stream, error);

    int ram_id = GetNameRamIdFromStack(tables, NODE_NAME(node));
    if (ram_id == NO_RAM)
    {
        int id = InsertNameInTable(LOCAL_TABLE(tables), NODE_NAME(node->left));
        int ram_id = *ram_spot++;

        LOCAL_TABLE(tables)->list[id].ram_id = ram_id;
    }

    fprintf(out_stream, "pop [%d]\n", ram_id);
}
