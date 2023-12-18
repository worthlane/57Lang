#include <stdio.h>

#include "backend.h"

static void TranslateToAsm(const tree_t* tree, const Node* node, FILE* out_stream);

//-----------------------------------------------------------------------------------------------------

void TranslateToAsm(const tree_t* tree, FILE* out_stream)
{
    assert(tree);

    TranslateToAsm(tree, tree->root, out_stream);
}

//-----------------------------------------------------------------------------------------------------

static void TranslateToAsm(const tree_t* tree, const Node* node, FILE* out_stream)
{
    assert(tree);


}
