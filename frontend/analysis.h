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

union TokenInfo
{
    int       val;
    int       var;
    Operators  keyword;  // TODO у меня был отдельный enum Keywords, который по факту повторял Operators, так будто централизованней система
    Operators opt;
};

struct token_t
{
    TokenType type;
    TokenInfo info;
    size_t    line;
};

void FillToken(token_t* token, const TokenType type, const TokenInfo info, const size_t line);

void DumpToken(FILE* fp, token_t* token);

// ======================================================================
// SYNTAX STORAGE
// ======================================================================

struct LexisStorage
{
    token_t*    tokens;
    size_t      size;

    nametable_t names;
};

void SyntaxStorageCtor(LexisStorage* storage);
void SyntaxStorageDtor(LexisStorage* storage);

void DumpSyntaxStorage(FILE* fp, LexisStorage* storage);

FrontendErrors Tokenize(LinesStorage* text, LexisStorage* storage, error_t* error);

#endif
