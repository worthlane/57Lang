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

void DumpNametable(FILE* fp, const nametable_t* nametable)
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

void CopyNametable(const nametable_t* nametable, nametable_t* dest)
{
    assert(nametable);
    assert(dest);

    dest->size     = nametable->size;
    dest->capacity = nametable->capacity;

    for (int i = 0; i < nametable->size; i++)
    {
        dest->list[i].type = nametable->list[i].type;
        dest->list[i].name = strndup(nametable->list[i].name, MAX_NAME_LEN);
    }
}

//-----------------------------------------------------------------------------------------------------

int InsertNameInTable(nametable_t* nametable, const char* name, const TokenType type)
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
    nametable->list[nametable->size].type = type;

    return nametable->size++;
}

//-----------------------------------------------------------------------------------------------------

#ifdef CHECK_OP
#undef CHECK_OP
#endif
#define CHECK_OP(opt)        \
    case (Operators::opt):        \
        fprintf(fp, #opt);   \
        break;                    \

void PrintOperator(FILE* fp, const Operators sign)
{
    switch (sign)
    {
        CHECK_OP(ADD);
        CHECK_OP(SUB);
        CHECK_OP(MUL);
        CHECK_OP(DIV);
        CHECK_OP(DEG);
        CHECK_OP(COMMA);
        CHECK_OP(L_BRACKET);
        CHECK_OP(R_BRACKET);

        CHECK_OP(BREAK);
        CHECK_OP(INPUT);
        CHECK_OP(INT);
        CHECK_OP(OUTPUT);
        CHECK_OP(SIN);
        CHECK_OP(COS);
        CHECK_OP(ASSIGN);
        CHECK_OP(IF);
        CHECK_OP(WHILE);
        CHECK_OP(FUNC_WALL);
        CHECK_OP(END);
        CHECK_OP(AND);
        CHECK_OP(OR);
        CHECK_OP(GREATER);
        CHECK_OP(GREATEREQUAL);
        CHECK_OP(LESSEQUAL);
        CHECK_OP(LESS);
        CHECK_OP(EQUAL);
        CHECK_OP(NOT_EQUAL);
        CHECK_OP(RETURN);
        CHECK_OP(ELSE);
        CHECK_OP(CLOSE_BLOCK);
        CHECK_OP(TYPE);
        CHECK_OP(FUNC);
        CHECK_OP(NEW_FUNC);
        CHECK_OP(FUNC_CALL);

        default:
            fprintf(fp, " undefined_operator ");
    }
}

#undef CHECK_SHORT_OP

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


Operators TranslateKeywordToOperator(const char* keyword)
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

#ifdef COMPARE_WORD
#undef COMPARE_WORD
#endif
#define COMPARE_WORD(op)                     \
    if (!strncmp(word, #op, MAX_NAME_LEN))   \
    {                                           \
        return Operators::op;                       \
    }                                           \
    else


Operators GetOperator(const char* word)
{
    if (!word) return Operators::UNK;

    COMPARE_WORD(ADD);
    COMPARE_WORD(SUB);
    COMPARE_WORD(MUL);
    COMPARE_WORD(DIV);
    COMPARE_WORD(DEG);

    COMPARE_WORD(ASSIGN);
    COMPARE_WORD(SIN);
    COMPARE_WORD(COS);
    COMPARE_WORD(IF);
    COMPARE_WORD(ELSE);
    COMPARE_WORD(WHILE);

    COMPARE_WORD(GREATER);
    COMPARE_WORD(GREATEREQUAL);
    COMPARE_WORD(LESS);
    COMPARE_WORD(LESSEQUAL);
    COMPARE_WORD(EQUAL);
    COMPARE_WORD(NOT_EQUAL);
    COMPARE_WORD(AND);
    COMPARE_WORD(OR);

    COMPARE_WORD(INPUT);
    COMPARE_WORD(OUTPUT);

    COMPARE_WORD(INT);

    COMPARE_WORD(L_BRACKET);
    COMPARE_WORD(R_BRACKET);
    COMPARE_WORD(COMMA);
    COMPARE_WORD(BREAK);
    COMPARE_WORD(CLOSE_BLOCK);
    COMPARE_WORD(FUNC_WALL);

    COMPARE_WORD(RETURN);

    COMPARE_WORD(END);

    COMPARE_WORD(FUNC_CALL);

    COMPARE_WORD(TYPE);
    COMPARE_WORD(NEW_FUNC);
    COMPARE_WORD(FUNC);

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

//-----------------------------------------------------------------------------------------------------

bool FindNameInTable(const nametable_t* nametable, const char* name, bool* exists, bool* is_func)
{
    assert(nametable);
    assert(name);
    assert(is_func);
    assert(exists);

    for (int i = 0; i < nametable->size; i++)
    {
        if (!strncmp(name, nametable->list[i].name, MAX_NAME_LEN))
        {
            if (nametable->list[i].type == TokenType::FUNC_NAME)
                *is_func = true;
            *exists = true;
            return true;
        }
    }

    return false;
}



