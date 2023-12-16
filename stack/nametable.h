#ifndef _NAMETABLE_H_
#define _NAMETABLE_H_

#include <stdio.h>

#include "tree/tree.h"

static const size_t DEFAULT_NAMES_AMT  = 64;
static const size_t MAX_NAME_LEN       = 64;

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

#endif
