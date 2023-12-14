#ifndef __ANALYSIS_H_
#define __ANALYSIS_H_

#include <stdio.h>

#include "tree/tree.h"
#include "common/file_read.h"
#include "frontend.h"

static const size_t DEFAULT_TOKENS_AMT = 500;
static const size_t DEFAULT_NAMES_AMT  = 64;

// ======================================================================
// NAMETABLE
// ======================================================================

enum class TokenType
{
    OP,
    KEYWORD,
    VAR,
    NUM,

    POISON
};

struct name_t
{
    char*     name;

    TokenType type;
};

struct nametable_t
{
    name_t* list;

    size_t size;
    size_t capacity;            // TODO пока не используется, переделать с реаллоком
};

void NametableCtor(nametable_t* nametable);
void NametableDtor(nametable_t* nametable);

void DumpNametable(FILE* fp, nametable_t* nametable);

int InsertNameInTable(nametable_t* nametable, const char* name);

// ======================================================================
// TOKEN
// ======================================================================

union TokenInfo
{
    int       val;
    int       var;
    Keywords  keyword;
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
