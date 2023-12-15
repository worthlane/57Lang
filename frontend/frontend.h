#ifndef __FRONTEND_H_
#define __FRONTEND_H_

#include "tree/tree.h"

static const size_t MAX_NAME_LEN = 64;

// ======================================================================
// ERRORS
// ======================================================================

enum class FrontendErrors
{
    NONE,

    INVALID_SYNTAX,
    UNKNOWN_OPERATOR,

    UNKNOWN
};

int PrintFrontendError(FILE* fp, const void* err, const char* func, const char* file, const int line);

#ifdef EXIT_IF_FRONTEND_ERROR
#undef EXIT_IF_FRONTEND_ERROR
#endif
#define EXIT_IF_FRONTEND_ERROR              do                                                            \
                                            {                                                           \
                                                if ((error).code != (int) FrontendErrors::NONE)      \
                                                {                                                       \
                                                    return LogDump(PrintFrontendError, &error, __func__,     \
                                                                    __FILE__, __LINE__);                \
                                                }                                                       \
                                            } while(0)
#ifdef RETURN_IF_FRONTEND_ERROR
#undef RETURN_IF_FRONTEND_ERROR
#endif
#define RETURN_IF_FRONTEND_ERROR            do                                                            \
                                            {                                                           \
                                                if (((error)->code) != (int) FrontendErrors::NONE)                  \
                                                {                                                       \
                                                    return (FrontendErrors) ((error)->code);                                       \
                                                }                                                       \
                                            } while(0)

#ifdef SET_FRONTEND_ERROR
#undef SET_FRONTEND_ERROR
#endif
#define SET_FRONTEND_ERROR(id, string, ...)                                                        \
                                            do                                                          \
                                            {                                                           \
                                                (error)->code = (int) FrontendErrors::id;               \
                                                SetErrorData(error, string, __VA_ARGS__);               \
                                                return (FrontendErrors) (error)->code;                  \
                                            } while(0)

// ======================================================================
// KEYWORDS
// ======================================================================

static const char* IF           = "57?";
static const char* ELSE         = "!57?";

static const char* INPUT        = "57>>";
static const char* OUTPUT       = "57<<";

static const char* INT          = "%";

static const char* WHILE        = "1000_7";
static const char* RETURN       = "~57";
static const char* SIN          = "_$1#_";
static const char* COS          = "_<0$_";
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
