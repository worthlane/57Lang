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

enum class Keywords
{
    IF,
    WHILE,
    ASSIGN,
    SIN,
    COS,
};

static const char* IF        = "57?";
static const char* WHILE     = "1000_7";
static const char* SIN       = "_57$0_";
static const char* COS       = "_57$1_";
static const char* ASSIGN    = ":=";

Operators TranslateKeywordToOperator(const char* keyword);

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

#endif
