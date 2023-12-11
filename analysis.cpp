#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "analysis.h"
#include "common/file_read.h"
#include "common/errors.h"

enum class CharType
{
    DIGIT,
    ALPHA,
    ADDITIONAL,
    OPT,

    UNK,
};

static CharType GetCharType(LinesStorage* text);
static void     SkipSymbols(LinesStorage* text);

static bool TokenizeOperator(LinesStorage* text, SyntaxStorage* storage);
static bool TokenizeWord(LinesStorage* text, SyntaxStorage* storage);
static bool TokenizeNumber(LinesStorage* text, SyntaxStorage* storage);

static bool CanCharBeInWord(const int ch);

static void ReadWord(LinesStorage* text, char* buffer);

static Digits ReadDigit(LinesStorage* text, char* buffer);

//-----------------------------------------------------------------------------------------------------

void Tokenize(LinesStorage* text, SyntaxStorage* storage, error_t* error)
{
    assert(text);
    assert(storage);
    assert(error);

    while(text->ptr < text->text_len)
    {
        SkipSymbols(text);

        CharType type = GetCharType(text);

        if (type == CharType::DIGIT || type == CharType::ADDITIONAL)
        {
            bool success = TokenizeWord(text, storage);
        }
        else if (type == CharType::OPT)
        {
            bool success = TokenizeOperator(text, storage);
        }
        else if (type == CharType::ALPHA)
        {
            bool success = TokenizeNumber(text, storage);
        }
    }
}

//-----------------------------------------------------------------------------------------------------

#ifdef SAVE_IF_SAME
#undef SAVE_IF_SAME
#endif
#define SAVE_IF_SAME(key)                                       \
        if (strncmp(buffer, key, MAX_NAME_LEN) == 0)            \
        {                                                       \
            token->type         = TokenType::KEYWORD;                   \
            token->info.keyword = Keywords::key;                 \
            token->line         = line;                            \
            storage->size++; \
            return true;                                        \
        }

static bool TokenizeWord(LinesStorage* text, SyntaxStorage* storage)
{
    assert(text);
    assert(storage);

    size_t line = text->curr_line;

    token_t* token = &storage->tokens[storage->size];

    char buffer[MAX_NAME_LEN] = "";
    ReadWord(text, buffer);

    SAVE_IF_SAME(IF);
    SAVE_IF_SAME(WHILE);
    SAVE_IF_SAME(ASSIGN);
    SAVE_IF_SAME(L_BRACKET);
    SAVE_IF_SAME(R_BRACKET);

    int id = InsertNameInTable(&storage->names, buffer);

    token->type     = TokenType::VAR;
    token->info.var = id;
    token->line     = line;
    storage->size++;

    return true;
}

#undef SAVE_IF_SAME

//-----------------------------------------------------------------------------------------------------

static bool TokenizeNumber(LinesStorage* text, SyntaxStorage* storage)
{
    assert(text);
    assert(storage);

    size_t   line  = text->curr_line;
    token_t* token = &storage->tokens[storage->size];

    char buffer[MAX_NAME_LEN] = "";

    int val = ReadDigit(text, buffer);
    if (val == Digits::UNK)
        return false;

    int ch = Bufgetc(text);
    while (ch == '_')
    {
        int val_2 = ReadDigit(text, buffer);
        if (val_2 == Digits::UNK)
            return false;

        val = val * 10 + val_2;

        ch = Bufgetc(text);
    }
    Bufungetc(text);

    token->type     = TokenType::NUM;
    token->info.val = val;
    token->line     = line;
    storage->size++;

    return true;
}

//-----------------------------------------------------------------------------------------------------

#ifdef IS_DIGIT
#undef IS_DIGIT
#endif
#define IS_DIGIT(digit)                                       \
        if (strncasecmp(buffer, #digit, MAX_NAME_LEN) == 0)            \
        {                                                       \
            return Digits::digit;                                        \
        }

static Digits ReadDigit(LinesStorage* text, char* buffer)
{
    assert(text);
    assert(buffer);

    int i = 0;

    int ch = Bufgetc(text);

    while ((isalpha(ch)) && i < MAX_NAME_LEN)
    {
        buffer[i++] = ch;
        ch = Bufgetc(text);
    }
    buffer[i] = '\0';

    Bufungetc(text);

    IS_DIGIT(ZERO);
    IS_DIGIT(ONE);
    IS_DIGIT(TWO);
    IS_DIGIT(THREE);
    IS_DIGIT(FOUR);
    IS_DIGIT(FIVE);
    IS_DIGIT(SIX);
    IS_DIGIT(SEVEN);
    IS_DIGIT(EIGHT);
    IS_DIGIT(NINE);

    return Digits::UNK;
}

#undef IS_DIGIT

//-----------------------------------------------------------------------------------------------------

static void ReadWord(LinesStorage* text, char* buffer)
{
    assert(text);
    assert(buffer);

    int i = 0;

    int ch = Bufgetc(text);

    while ((CanCharBeInWord(ch)) && i < MAX_NAME_LEN)
    {
        buffer[i++] = ch;
        ch = Bufgetc(text);
    }
    buffer[i] = '\0';

    Bufungetc(text);
}

//-----------------------------------------------------------------------------------------------------

static bool CanCharBeInWord(const int ch)
{
    if ('0' <= ch && ch <= '9')
        return true;

    switch (ch)
    {
        case '$':
        case '=':
        case '£':
        case '.':
        case '?':
        case '_':
            return true;
        default:
            return false;
    }
}

//-----------------------------------------------------------------------------------------------------

static bool TokenizeOperator(LinesStorage* text, SyntaxStorage* storage)
{
    assert(text);
    assert(storage);

    size_t line = text->curr_line;
    int    ch   = Bufgetc(text);

    Operators op = Operators::UNK;

    switch(ch)
    {
        case '+':
            op = Operators::ADD;
            break;
        case '-':
            op = Operators::SUB;
            break;
        case '*':
            op = Operators::MUL;
            break;
        case '/':
            op = Operators::DIV;
            break;
        default:
            return false;
    }

    token_t* token = &storage->tokens[storage->size];

    token->info.opt = op;
    token->line     = line;
    token->type     = TokenType::OP;

    storage->size++;

    return true;
}

//-----------------------------------------------------------------------------------------------------

static void SkipSymbols(LinesStorage* text)
{
    assert(text);

    bool need_to_skip = true;

    while (need_to_skip)
    {
        need_to_skip = false;

        size_t old_ptr = text->ptr;

        SkipBufSpaces(text);

        if (text->ptr != old_ptr)
            need_to_skip = true;

        int ch_1 = Bufgetc(text);
        int ch_2 = Bufgetc(text);

        if (ch_1 == '/' && ch_2 == '/')
        {
            int ch = Bufgetc(text);

            while (ch != '\0' && ch != '\n')
                ch = Bufgetc(text);

            need_to_skip = true;
        }
        else
        {
            Bufungetc(text);
            Bufungetc(text);
        }
    }
}

//-----------------------------------------------------------------------------------------------------

static CharType GetCharType(LinesStorage* text)
{
    assert(text);

    int ch = Bufgetc(text);
    Bufungetc(text);

    if (isalpha(ch))
        return CharType::ALPHA;

    if ('0' <= ch && ch <= '9')
        return CharType::DIGIT;

    switch (ch)
    {
        case '+':
        case '-':
        case '*':
        case '/':
            return CharType::OPT;
        case '=':
        case '(':
        case ')':
        case '{':
        case '}':
        case '[':
        case ']':
        case '_':
        case ',':
        case '&':
        case '|':
            return CharType::ADDITIONAL;
        default:
            return CharType::UNK;
    }


}

//-----------------------------------------------------------------------------------------------------

void NametableCtor(nametable_t* nametable)
{
    assert(nametable);

    char** names = (char**) calloc(DEFAULT_NAMES_AMT, sizeof(char*));

    assert(names);

    nametable->names    = names;
    nametable->size     = 0;
    nametable->capacity = DEFAULT_NAMES_AMT;
}

//-----------------------------------------------------------------------------------------------------

void NametableDtor(nametable_t* nametable)
{
    assert(nametable);
    assert(nametable->names);

    for (int i = 0; i < nametable->size; i++)
    {
        if (nametable->names[i] != nullptr)
            free(nametable->names[i]);
    }

    free(nametable->names);

    nametable->size     = 0;
    nametable->capacity = 0;
}

//-----------------------------------------------------------------------------------------------------

void DumpNametable(FILE* fp, nametable_t* nametable)
{
    assert(nametable);
    assert(nametable->names);

    for (int i = 0; i < nametable->size; i++)
    {
        if (nametable->names[i] != nullptr)
            fprintf(fp, "\"%s\"[%d]\n", nametable->names[i], i);
    }
}

//-----------------------------------------------------------------------------------------------------

int InsertNameInTable(nametable_t* nametable, const char* name)
{
    assert(nametable);
    assert(nametable->names);
    assert(name);

    for (int i = 0; i < nametable->size; i++)
    {
        if (!strncmp(name, nametable->names[i], MAX_NAME_LEN))
            return i;
    }

    char* inserted_name = strndup(name, MAX_NAME_LEN);
    assert(inserted_name);

    nametable->names[nametable->size] = inserted_name;

    return nametable->size++;
}

//-----------------------------------------------------------------------------------------------------

void FillToken(token_t* token, const TokenType type, const TokenInfo info, const size_t line)
{
    assert(token);

    token->type = type;
    token->info = info;
    token->line = line;
}

//-----------------------------------------------------------------------------------------------------

void DumpToken(FILE* fp, token_t* token)
{
    assert(token);

    fprintf(fp, "TYPE: %d\n"
                "INFO: %d\n"
                "LINE: %d\n", token->type, token->info, token->line);
}

//-----------------------------------------------------------------------------------------------------

void SyntaxStorageCtor(SyntaxStorage* storage)
{
    assert(storage);

    token_t* tokens = (token_t*) calloc(DEFAULT_TOKENS_AMT, sizeof(token_t));

    assert(tokens);

    storage->tokens = tokens;
    storage->size   = 0;

    NametableCtor(&storage->names);
}

//-----------------------------------------------------------------------------------------------------

void SyntaxStorageDtor(SyntaxStorage* storage)
{
    assert(storage);

    free(storage->tokens);

    storage->size   = 0;

    NametableDtor(&storage->names);
}

//-----------------------------------------------------------------------------------------------------

void DumpSyntaxStorage(FILE* fp, SyntaxStorage* storage)
{
    assert(storage);

    for (int i = 0; i < storage->size; i++)
    {
        fprintf(fp, "[%d]{\n");
        DumpToken(fp, &storage->tokens[i]);
        fprintf(fp, "}\n");
    }

    fprintf(fp, "NAMETABLE\n");

    DumpNametable(fp, &storage->names);
}

