#include <string.h>

#include "frontend.h"

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

//-----------------------------------------------------------------------------------------------------

int PrintFrontendError(FILE* fp, const void* err, const char* func, const char* file, const int line)
{
    assert(err);

    LOG_START(func, file, line);

    const struct ErrorInfo* error = (const struct ErrorInfo*) err;

    switch ((FrontendErrors) error->code)
    {
        case (FrontendErrors::NONE):
            LOG_END();
            return (int) error->code;

        case (FrontendErrors::INVALID_SYNTAX):
            fprintf(fp, "INVALID SYNTAX:<br>\n"
                        "%s<br>\n", (const char*) error->data);
            LOG_END();
            return (int) FrontendErrors::UNKNOWN;

        case (FrontendErrors::UNKNOWN_OPERATOR):
            fprintf(fp, "UNKNOWN_OPERATOR:<br>\n"
                        "%s<br>\n", (const char*) error->data);
            LOG_END();
            return (int) FrontendErrors::UNKNOWN;

        case (FrontendErrors::UNKNOWN):
        // fall through
        default:
            fprintf(fp, "UNKNOWN FRONTEND ERROR<br>\n");
            LOG_END();
            return (int) FrontendErrors::UNKNOWN;
    }
}
