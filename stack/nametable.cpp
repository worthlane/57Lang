#include <assert.h>
#include <string.h>

#include "nametable.h"

//-----------------------------------------------------------------------------------------------------

void GlobalNametableCtor(nametable_t* nametable)
{
    assert(nametable);

    NametableCtor(nametable);

    FillNametableWithKeywords(nametable);
}

//-----------------------------------------------------------------------------------------------------

int InsertKeywordInTable(nametable_t* nametable, const char* name)
{
    int id = InsertNameInTable(nametable, name);

    nametable->list[id].type = TokenType::TOKEN;

    return id;
}

//-----------------------------------------------------------------------------------------------------

void FillNametableWithKeywords(nametable_t* nametable)
{
    assert(nametable);

    InsertKeywordInTable(nametable, IF);
    InsertKeywordInTable(nametable, ELSE);
    InsertKeywordInTable(nametable, CLOSE_BLOCK);
    InsertKeywordInTable(nametable, WHILE);
    InsertKeywordInTable(nametable, FUNC_WALL);
    InsertKeywordInTable(nametable, SIN);
    InsertKeywordInTable(nametable, INPUT);
    InsertKeywordInTable(nametable, OUTPUT);
    InsertKeywordInTable(nametable, INT);
    InsertKeywordInTable(nametable, COS);
    InsertKeywordInTable(nametable, ASSIGN);
    InsertKeywordInTable(nametable, END);
    InsertKeywordInTable(nametable, AND);
    InsertKeywordInTable(nametable, OR);
    InsertKeywordInTable(nametable, LESS);
    InsertKeywordInTable(nametable, LESSEQUAL);
    InsertKeywordInTable(nametable, GREATER);
    InsertKeywordInTable(nametable, NOT_EQUAL);
    InsertKeywordInTable(nametable, GREATEREQUAL);
    InsertKeywordInTable(nametable, EQUAL);
    InsertKeywordInTable(nametable, RETURN);
}

//-----------------------------------------------------------------------------------------------------

nametable_t* MakeNametable()
{
    nametable_t* names = (nametable_t*) calloc(1, sizeof(nametable_t));

    assert(names);

    NametableCtor(names);

    return names;
}

//-----------------------------------------------------------------------------------------------------

void NametableCtor(nametable_t* nametable)
{
    assert(nametable);

    name_t* list = (name_t*) calloc(DEFAULT_NAMES_AMT, sizeof(name_t));

    assert(list);

    nametable->list     = list;
    nametable->size     = 0;
    nametable->capacity = DEFAULT_NAMES_AMT;
}

//-----------------------------------------------------------------------------------------------------

void NametableDtor(nametable_t* nametable)
{
    assert(nametable);
    assert(nametable->list);

    for (int i = 0; i < nametable->size; i++)
    {
        if (nametable->list[i].name != nullptr)
            free(nametable->list[i].name);
    }

    free(nametable->list);

    nametable->size     = 0;
    nametable->capacity = 0;
}

//-----------------------------------------------------------------------------------------------------

void DumpNametable(FILE* fp, nametable_t* nametable)
{
    assert(nametable);
    assert(nametable->list);

    fprintf(fp, "NAMETABLE SIZE > %d\n", nametable->size);

    for (int i = 0; i < nametable->size; i++)
    {
        if (nametable->list[i].name != nullptr)
            fprintf(fp, "\"%s\"[%d]\n", nametable->list[i].name, i);
    }
}

//-----------------------------------------------------------------------------------------------------

int InsertNameInTable(nametable_t* nametable, const char* name)
{
    assert(nametable);
    assert(nametable->list);
    assert(name);

    for (int i = 0; i < nametable->size; i++)
    {
        if (!strncmp(name, nametable->list[i].name, MAX_NAME_LEN))
            return i;
    }
    char* inserted_name = strndup(name, MAX_NAME_LEN);
    assert(inserted_name);

    nametable->list[nametable->size].name = inserted_name;
    nametable->list[nametable->size].type = TokenType::NAME;

    return nametable->size++;
}

//-----------------------------------------------------------------------------------------------------

#ifdef COMPARE_KEYWORD
#undef COMPARE_KEYWORD
#endif
#define COMPARE_KEYWORD(op)                     \
    if (!strncmp(keyword, op, MAX_NAME_LEN))   \
    {                                           \
        return Operators::op;                       \
    }                                           \
    else


Operators TranslateKeywordToOperator(const char* keyword)  // TODO operators not like keyword
{
    if (!keyword) return Operators::UNK;

    COMPARE_KEYWORD(SIN);
    COMPARE_KEYWORD(COS);
    COMPARE_KEYWORD(ASSIGN);
    COMPARE_KEYWORD(IF);
    COMPARE_KEYWORD(END);
    COMPARE_KEYWORD(INPUT);
    COMPARE_KEYWORD(OUTPUT);
    COMPARE_KEYWORD(INT);
    COMPARE_KEYWORD(ELSE);
    COMPARE_KEYWORD(CLOSE_BLOCK);
    COMPARE_KEYWORD(WHILE);
    COMPARE_KEYWORD(GREATER);
    COMPARE_KEYWORD(GREATEREQUAL);
    COMPARE_KEYWORD(LESS);
    COMPARE_KEYWORD(LESSEQUAL);
    COMPARE_KEYWORD(EQUAL);
    COMPARE_KEYWORD(NOT_EQUAL);
    COMPARE_KEYWORD(RETURN);
    COMPARE_KEYWORD(AND);
    COMPARE_KEYWORD(OR);
    COMPARE_KEYWORD(FUNC_WALL);

    /* else */ return Operators::UNK;

}

#undef COMPARE_KEYWORD

//-----------------------------------------------------------------------------------------------------

bool IsType(const Operators type)
{
    switch (type)
    {
        case (Operators::INT):
            return true;
        default:
            return false;
    }

    return false;
}
