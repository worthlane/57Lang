#ifndef __SYNTAX_PARSER_H_
#define __SYNTAX_PARSER_H_

#include "analysis.h"

struct SyntaxStorage
{
    LexisStorage* lexis;
    size_t        ptr;
};

Node* GetAssign(SyntaxStorage* storage, error_t* error);

#endif
