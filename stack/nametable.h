#ifndef _NAMETABLE_H_
#define _NAMETABLE_H_

#include <stdio.h>

static const size_t DEFAULT_NAMES_AMT  = 64;
static const size_t MAX_NAME_LEN       = 64;

// ======================================================================
// OPERATORS
// ======================================================================

enum class Operators
{
    ADD,
    SUB,
    MUL,
    DIV,
    DEG,

    ASSIGN,
    SIN,
    COS,
    IF,
    ELSE,
    WHILE,

    GREATER,
    GREATEREQUAL,
    LESS,
    LESSEQUAL,
    EQUAL,
    NOT_EQUAL,
    AND,
    OR,

    INPUT,
    OUTPUT,

    INT,

    L_BRACKET,
    R_BRACKET,
    COMMA,
    BREAK,
    CLOSE_BLOCK,
    FUNC_WALL,

    RETURN,

    END,

    // FICTIVE OPERATORS:

    FUNC_CALL,

    TYPE,
    NEW_FUNC,
    FUNC,

    UNK
};

static const char ADD       = '+';
static const char SUB       = '-';
static const char MUL       = '*';
static const char DIV       = '/';
static const char DEG       = '^';
static const char L_BRACKET = '(';
static const char R_BRACKET = ')';
static const char COMMA     = ',';
static const char BREAK     = '\n';

// ======================================================================
// TOKENS
// ======================================================================

enum class TokenType
{
    NUM,
    TOKEN,
    NAME,

    FUNC_NAME,
    VAR_NAME,
};

union TokenValue
{
    Operators       opt;
    int             val;
    int             name_id;
};

struct token_t
{
    TokenType  type;
    TokenValue info;
    size_t     line;
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

nametable_t* MakeNametable();
void         NametableCtor(nametable_t* nametable);
void         NametableDtor(nametable_t* nametable);

void DumpNametable(FILE* fp, nametable_t* nametable);

int InsertNameInTable(nametable_t* nametable, const char* name);

// ======================================================================
// KEYWORDS
// ======================================================================

int  InsertKeywordInTable(nametable_t* nametable, const char* name);
void FillNametableWithKeywords(nametable_t* nametable);
void GlobalNametableCtor(nametable_t* nametable);

static const char* IF           = "57?";
static const char* ELSE         = "!57?";

static const char* INPUT        = "57>>";
static const char* OUTPUT       = "57<<";

static const char* INT          = "57::";

// static const char* MAIN         = "_0_";
static const char* WHILE        = "1000_7";
static const char* RETURN       = "~57";
static const char* SIN          = "$1#";
static const char* COS          = "<0$";
static const char* ASSIGN       = ":=";
static const char* GREATER      = ">";
static const char* GREATEREQUAL = ">=";
static const char* LESS         = "<";
static const char* LESSEQUAL    = "<=";
static const char* EQUAL        = "==";
static const char* NOT_EQUAL    = "!=";
static const char* AND          = "&&";
static const char* OR           = "||";
static const char* CLOSE_BLOCK  = ".57";
static const char* END          = "@57@";

static const char* FUNC_WALL    = "##################################################";

Operators TranslateKeywordToOperator(const char* keyword);
bool      IsType(const Operators type);

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

    UNK = -1        // TODO unknown
};

#endif
