#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "analysis.h"
#include "common/file_read.h"
#include "common/errors.h"
#include "tree/tree_output.h"

enum class CharType
{
    DIGIT,
    ALPHA,
    ADDITIONAL,
    OPT,

    UNK,
};

static CharType CheckBufCharType(LinesStorage* text);
static CharType GetCharType(const int ch);

static void     SkipSymbols(LinesStorage* text);
static bool     SkipComments(LinesStorage* text);
static bool     SkipSpaces(LinesStorage* text);

static bool TokenizeOperator(LinesStorage* text, LexisStorage* storage);
static bool TokenizeWord(LinesStorage* text, LexisStorage* storage);
static bool TokenizeNumber(LinesStorage* text, LexisStorage* storage);

static bool CanCharBeInWord(const int ch);

static void ReadWord(LinesStorage* text, char* buffer);

static Digits ReadDigit(LinesStorage* text, char* buffer);

static int InsertKeywordInTable(nametable_t* nametable, const char* name);
static void FillNametableWithKeywords(nametable_t* nametable);

//-----------------------------------------------------------------------------------------------------

void Tokenize(LinesStorage* text, LexisStorage* storage, error_t* error)
{
    assert(text);
    assert(storage);
    assert(error);

    while(text->ptr < text->text_len)
    {
        SkipSymbols(text);

        CharType type = CheckBufCharType(text);

        if (type == CharType::DIGIT || type == CharType::ADDITIONAL)
        {
            bool success = TokenizeWord(text, storage);
            assert(success);
        }
        else if (type == CharType::OPT)
        {
            bool success = TokenizeOperator(text, storage);
            assert(success);
        }
        else if (type == CharType::ALPHA)
        {
            bool success = TokenizeNumber(text, storage);
            assert(success);
        }
        else
        {
            PrintLog("ANALYSIS STOPPED. UNKNOWN SYMBOL IN LINE %d<br>\n", text->curr_line);
            abort();
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
            token->type         = TokenType::KEYWORD;           \
            token->info.keyword = Keywords::key;                \
            token->line         = line;                         \
            storage->size++;                                    \
            return true;                                        \
        }

static bool TokenizeWord(LinesStorage* text, LexisStorage* storage)
{
    assert(text);
    assert(storage);

    size_t line = text->curr_line;

    token_t* token = &storage->tokens[storage->size];

    char buffer[MAX_NAME_LEN] = "";
    ReadWord(text, buffer);

    int id = InsertNameInTable(&storage->names, buffer);

    token->type     = storage->names.list[id].type;
    token->info.var = id;
    token->line     = line;
    storage->size++;

    return true;
}

#undef SAVE_IF_SAME

//-----------------------------------------------------------------------------------------------------

static bool TokenizeNumber(LinesStorage* text, LexisStorage* storage)
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
    CharType type = GetCharType(ch);

    if (type == CharType::DIGIT || type == CharType::ADDITIONAL)
        return true;
    else
        return false;
}

//-----------------------------------------------------------------------------------------------------

#ifdef IS_OP
#undef IS_OP
#endif
#define IS_OP(opt)                                       \
        case opt:                                         \
            op = Operators::opt;                          \
            break;

static bool TokenizeOperator(LinesStorage* text, LexisStorage* storage)
{
    assert(text);
    assert(storage);

    size_t line = text->curr_line;
    int    ch   = Bufgetc(text);

    Operators op = Operators::UNK;

    switch(ch)
    {
        IS_OP(ADD);
        IS_OP(SUB);
        IS_OP(MUL);
        IS_OP(DIV);
        IS_OP(DEG);
        IS_OP(COMMA);
        IS_OP(L_BRACKET);
        IS_OP(R_BRACKET);
        IS_OP(BREAK);
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

#undef IS_OP

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
    COMPARE_KEYWORD(WHILE);

    /* else */ return Operators::UNK;

}

#undef COMPARE_KEYWORD

//-----------------------------------------------------------------------------------------------------

static void SkipSymbols(LinesStorage* text)
{
    assert(text);

    bool need_to_skip = true;

    while (need_to_skip)
        need_to_skip = SkipSpaces(text) || SkipComments(text);

}

//-----------------------------------------------------------------------------------------------------

static bool SkipSpaces(LinesStorage* info)
{
    int ch       = 0;
    bool skipped = false;

    ch = Bufgetc(info);

    while (ch == ' ' || ch == '\t')
    {
        ch = Bufgetc(info);
        skipped = true;
    }

    Bufungetc(info);

    return skipped;
}

//-----------------------------------------------------------------------------------------------------

static bool SkipComments(LinesStorage* text)
{
    int ch_1 = Bufgetc(text);
    int ch_2 = Bufgetc(text);
    bool skipped = false;

    if (ch_1 == '/' && ch_2 == '/')
    {
        int ch = Bufgetc(text);

        while (ch != '\0' && ch != '\n')
            ch = Bufgetc(text);

        skipped = true;
    }
    else
    {
        Bufungetc(text);
        Bufungetc(text);
    }

    return skipped;
}

//-----------------------------------------------------------------------------------------------------

static CharType CheckBufCharType(LinesStorage* text)
{
    assert(text);

    int ch = Bufgetc(text);
    Bufungetc(text);

    return GetCharType(ch);
}

//-----------------------------------------------------------------------------------------------------

static CharType GetCharType(const int ch)
{
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
        case '^':
        case '(':
        case ')':
        case '\n':
        case ',':
            return CharType::OPT;
        case '$':
        case '=':
        case '.':
        case '?':
        case '_':
        case ':':
        case '&':
        case '|':
        case '<':
        case '>':
            return CharType::ADDITIONAL;
        default:
            return CharType::UNK;
    }
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

    FillNametableWithKeywords(nametable);
}

//-----------------------------------------------------------------------------------------------------

static void FillNametableWithKeywords(nametable_t* nametable)
{
    assert(nametable);

    InsertKeywordInTable(nametable, IF);
    InsertKeywordInTable(nametable, WHILE);
    InsertKeywordInTable(nametable, SIN);
    InsertKeywordInTable(nametable, COS);
    InsertKeywordInTable(nametable, ASSIGN);
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
    nametable->list[nametable->size].type = TokenType::VAR;

    return nametable->size++;
}

//-----------------------------------------------------------------------------------------------------

static int InsertKeywordInTable(nametable_t* nametable, const char* name)
{
    int id = InsertNameInTable(nametable, name);

    nametable->list[id].type = TokenType::KEYWORD;

    return id;
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

    switch (token->type)
    {
        case TokenType::KEYWORD:
            fprintf(fp, "TYPE > KEYWORD\n"
                        "ID   > %d\n"
                        "LINE > %d\n", token->info.var, token->line);
            return;
        case TokenType::NUM:
            fprintf(fp, "TYPE > NUMBER\n"
                        "VAL  > %d\n"
                        "LINE > %d\n", token->info.val, token->line);
            return;
        case TokenType::OP:
            fprintf(fp, "TYPE > OPERATOR ");
            PrintOperator(fp, token->info.opt);
            fprintf(fp, "\nLINE > %d\n", token->line);
            return;
        case TokenType::VAR:
            fprintf(fp, "TYPE > VARIABLE\n"
                        "ID   > %d\n"
                        "LINE > %d\n", token->info.var, token->line);
            return;
        case TokenType::POISON:
        default:
            fprintf(fp, "POISONED TOKEN\n");
            return;
    }
}

//-----------------------------------------------------------------------------------------------------

void SyntaxStorageCtor(LexisStorage* storage)
{
    assert(storage);

    token_t* tokens = (token_t*) calloc(DEFAULT_TOKENS_AMT, sizeof(token_t));

    assert(tokens);

    storage->tokens = tokens;
    storage->size   = 0;

    NametableCtor(&storage->names);
}

//-----------------------------------------------------------------------------------------------------

void SyntaxStorageDtor(LexisStorage* storage)
{
    assert(storage);

    free(storage->tokens);

    storage->size   = 0;

    NametableDtor(&storage->names);
}

//-----------------------------------------------------------------------------------------------------

void DumpSyntaxStorage(FILE* fp, LexisStorage* storage)
{
    assert(storage);

    for (int i = 0; i < storage->size; i++)
    {
        fprintf(fp, "[%d]{\n", i);
        DumpToken(fp, &storage->tokens[i]);
        fprintf(fp, "}\n");
    }

    DumpNametable(fp, &storage->names);
}

