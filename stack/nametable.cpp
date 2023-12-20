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

#define DEF_OP(name, is_char, symb)             \
        if (!is_char)                           \
        {                                       \
            InsertKeywordInTable(nametable, symb);  \
        }                                               \

void FillNametableWithKeywords(nametable_t* nametable)
{
    assert(nametable);

    #include "common/operations.h"
}

#undef DEF_OP

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

#define DEF_OP(name, ...)        \
    case (Operators::name):        \
        fprintf(fp, #name);   \
        break;                    \

void PrintOperator(FILE* fp, const Operators sign)
{
    switch (sign)
    {
        #include "common/operations.h"

        case (Operators::TYPE):
            fprintf(fp, "TYPE");
            break;

        case (Operators::FUNC):
            fprintf(fp, "FUNC");
            break;

        case (Operators::NEW_FUNC):
            fprintf(fp, "NEW_FUNC");
            break;

        case (Operators::FUNC_CALL):
            fprintf(fp, "FUNC_CALL");
            break;

        default:
            fprintf(fp, " undefined_operator ");
    }
}

#undef DEF_OP

//-----------------------------------------------------------------------------------------------------

#define DEF_OP(name, is_char, symb)                     \
    if (!strncmp(keyword, symb, MAX_NAME_LEN))   \
    {                                           \
        return Operators::name;                       \
    }                                           \
    else


Operators TranslateKeywordToOperator(const char* keyword)
{
    if (!keyword) return Operators::UNK;

    #include "common/operations.h"

    /* else */ return Operators::UNK;

}

#undef DEF_OP

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



