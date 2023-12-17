#ifndef __ANALYSIS_H_
#define __ANALYSIS_H_

#include <stdio.h>

#include "tree/tree.h"
#include "common/file_read.h"
#include "frontend.h"
#include "stack/nametable.h"

static const size_t DEFAULT_TOKENS_AMT = 500;

// ======================================================================
// TOKEN
// ======================================================================

struct token_t
{
    NodeType  type;
    NodeValue info;
    size_t    line;
};

void FillToken(token_t* token, const NodeType type, const NodeValue info, const size_t line);

void DumpToken(FILE* fp, token_t* token);

// ======================================================================
// SYNTAX STORAGE
// ======================================================================

struct Tokens
{
    token_t*    array;
    size_t      size;

    nametable_t names;
};

void SyntaxStorageCtor(Tokens* storage);
void SyntaxStorageDtor(Tokens* storage);

void DumpSyntaxStorage(FILE* fp, Tokens* storage);

FrontendErrors Tokenize(LinesStorage* text, Tokens* storage, error_t* error);

#endif
