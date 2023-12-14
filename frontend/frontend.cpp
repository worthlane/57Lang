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
