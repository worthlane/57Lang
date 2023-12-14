#include <stdio.h>

#include "common/logs.h"
#include "common/input_and_output.h"
#include "common/file_read.h"
#include "tree/tree.h"
#include "tree/tree_output.h"
#include "frontend/analysis.h"
#include "frontend/syntax_parser.h"
#include "frontend/frontend.h"

int main(const int argc, const char* argv[])
{
    OpenLogFile(argv[0]);

    error_t error = {};
    tree_t tree = {};

    const char* data_file = GetFileName(argc, argv, 1, "INPUT", &error);
    EXIT_IF_ERROR(&error);
    FILE* fp = OpenInputFile(data_file, &error);
    EXIT_IF_ERROR(&error);

    LinesStorage info = {};
    CreateTextStorage(&info, &error, data_file);

    LexisStorage storage = {};
    SyntaxStorageCtor(&storage);

    Tokenize(&info, &storage, &error);
    EXIT_IF_FRONTEND_ERROR;

    // DumpSyntaxStorage(stdout, &storage);

    TreeCtor(&tree, &error);

    SyntaxStorage syn = {};
    syn.ptr = 0;
    syn.lexis = &storage;

    tree.root = GetAssign(&syn, &error);
    EXIT_IF_FRONTEND_ERROR;

    DUMP_TREE(&tree);
}
