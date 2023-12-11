#ifndef __ANALYSIS_H_
#define __ANALYSIS_H_

#include <stdio.h>

#include "tree/tree.h"
#include "common/file_read.h"

static const size_t DEFAULT_TOKENS_AMT = 500;
static const size_t DEFAULT_NAMES_AMT  = 64;
static const size_t MAX_NAME_LEN       = 64;

// ======================================================================
// NAMETABLE
// ======================================================================

struct nametable_t
{
    char** names;

    size_t size;
    size_t capacity;
};

void NametableCtor(nametable_t* nametable);
void NametableDtor(nametable_t* nametable);

void DumpNametable(FILE* fp, nametable_t* nametable);

int InsertNameInTable(nametable_t* nametable, const char* name);

// ======================================================================
// KEYWORDS
// ======================================================================

enum class Keywords
{
    IF,
    WHILE,
    ASSIGN,
    L_BRACKET,
    R_BRACKET,
    L_BLOCK,
    R_BLOCK,
    SIN,
    COS,
};

static const char* IF        = "57?";
static const char* WHILE     = "57...";
static const char* L_BRACKET = "5";
static const char* R_BRACKET = "7";
static const char* L_BLOCK   = "{";
static const char* R_BLOCK   = "}";
static const char* SIN       = "57$";
static const char* COS       = "57£";
static const char* ASSIGN    = "5=7";

// ======================================================================
// WORD DIGITS FORMAT
// ======================================================================

enum Digits
{
    ZERO  = 0,
    ONE   = 1,
    TWO   = 2,
    THREE = 3,
    FOUR  = 4,
    FIVE  = 5,
    SIX   = 6,
    SEVEN = 7,
    EIGHT = 8,
    NINE  = 9,

    UNK = -1
};

// ======================================================================
// TOKEN
// ======================================================================

enum class TokenType
{
    OP,
    KEYWORD,
    VAR,
    NUM,

    POISON
};

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

struct SyntaxStorage
{
    token_t*    tokens;
    size_t      size;

    nametable_t names;
};

void SyntaxStorageCtor(SyntaxStorage* storage);
void SyntaxStorageDtor(SyntaxStorage* storage);

void DumpSyntaxStorage(FILE* fp, SyntaxStorage* storage);

void Tokenize(LinesStorage* text, SyntaxStorage* storage, error_t* error);

#endif
