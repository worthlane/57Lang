#ifndef __SYNTAX_PARSER_H_
#define __SYNTAX_PARSER_H_

#include "analysis.h"

struct SyntaxStorage
{
    LexisStorage* lexis;
    size_t        ptr;
};

void GetTreeFromTokens(LexisStorage* lexis, tree_t* tree, error_t* error);

#endif
