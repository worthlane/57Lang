#ifndef DEF_OP
#define DEF_OP(...) ;
#endif

// ======================================================================
// FORMAT
// ======================================================================

// DEF_OP(NAME, IS_CHAR, SYMBOLS, ACTION, IS_CALC, ASM)

// ======================================================================
// OPERATIONS
// ======================================================================

DEF_OP(ADD, true, "+", (NUMBER_1 + NUMBER_2), true,
{
    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);
    TranslateNodeToAsm(tree, node->right, tables, out_stream, error);
    fprintf(out_stream, "add\n");
    break;
})

DEF_OP(SUB, true, "-", (NUMBER_1 - NUMBER_2), true,
{
    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);
    TranslateNodeToAsm(tree, node->right, tables, out_stream, error);
    fprintf(out_stream, "sub\n");
    break;
})

DEF_OP(MUL, true, "*", (NUMBER_1 * NUMBER_2), true,
{
    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);
    TranslateNodeToAsm(tree, node->right, tables, out_stream, error);
    fprintf(out_stream, "mul\n");
    break;
})

DEF_OP(DIV, true, "/", (NUMBER_1 / NUMBER_2), true,
{
    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);
    TranslateNodeToAsm(tree, node->right, tables, out_stream, error);
    fprintf(out_stream, "div\n");
    break;
})

DEF_OP(DEG, true, "^", ( pow(NUMBER_1, NUMBER_2) ), true,
{
    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);
    TranslateNodeToAsm(tree, node->right, tables, out_stream, error);
    fprintf(out_stream, "pow\n");
    break;
})

DEF_OP(ASSIGN, false, ":=", (0), false,
{
    TranslateAssignToAsm(tree, node, tables, &ram_spot, out_stream, error);
    break;
})

DEF_OP(SIN, false, "$1#", ( sin( NUMBER_1 )), true,
{
    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);
    fprintf(out_stream, "sin\n");
    break;
})

DEF_OP(SQRT, false, "57#", ( sqrt( NUMBER_1 )), true,
{
    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);
    fprintf(out_stream, "sqrt\n");
    break;
})

DEF_OP(COS, false, "<0$", ( cos( NUMBER_1 )), true,
{
    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);
    fprintf(out_stream, "cos\n");
    break;
})

DEF_OP(IF, false, "57?", (0), false,
{
    int free_label = label_spot++;

    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);

    fprintf(out_stream, "push 0      %% IF START\n"
                        "je :FALSE_COND_%d\n", (int) free_label);

    nametable_t* local = MakeNametable();
    StackPush(tables, local);

    TranslateNodeToAsm(tree, node->right, tables, out_stream, error);

    StackPop(tables);

    fprintf(out_stream, ":FALSE_COND_%d      %% IF END\n", (int) free_label);

    break;
})

DEF_OP(WHILE, false, "1000_7", (0), false,
{
    int free_label_cycle = label_spot++;
    int free_label_cond  = label_spot++;

    fprintf(out_stream, ":WHILE_CYCLE_%d   %% WHILE START\n", free_label_cycle);

    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);

    fprintf(out_stream, "push 0\n"
                                "je :QUIT_CYCLE_%d\n", free_label_cond);

    nametable_t* local = MakeNametable();
    StackPush(tables, local);

    TranslateNodeToAsm(tree, node->right, tables, out_stream, error);

    StackPop(tables);

    fprintf(out_stream, "jmp :WHILE_CYCLE_%d\n"
                        ":QUIT_CYCLE_%d       %% WHILE END\n", free_label_cycle, free_label_cond);

    break;
})

DEF_OP(GREATER, false,">", (NUMBER_1 > NUMBER_2), true,
{
    TranslateCompareToAsm(tree, node, tables, &label_spot, "jb", out_stream, error);
    break;
})
DEF_OP(GREATER_EQ, false, ">=", (NUMBER_1 >= NUMBER_2), true,
{
    TranslateCompareToAsm(tree, node, tables, &label_spot, "jbe", out_stream, error);
    break;
})
DEF_OP(LESS, false, "<", (NUMBER_1 < NUMBER_2), true,
{
    TranslateCompareToAsm(tree, node, tables, &label_spot, "ja", out_stream, error);
    break;
})
DEF_OP(LESS_EQ, false, "<=", (NUMBER_1 <= NUMBER_2), true,
{
    TranslateCompareToAsm(tree, node, tables, &label_spot, "jae", out_stream, error);
    break;
})
DEF_OP(EQ,false, "==", (NUMBER_1 == NUMBER_2), true,
{
    TranslateCompareToAsm(tree, node, tables, &label_spot, "je", out_stream, error);
    break;
})
DEF_OP(NOT_EQ, false, "!=", (NUMBER_1 != NUMBER_2), true,
{
    TranslateCompareToAsm(tree, node, tables, &label_spot, "jne", out_stream, error);
    break;
})
DEF_OP(AND, false, "&&", (NUMBER_1 && NUMBER_2), true,
{
    TranslateAndToAsm(tree, node, tables, &label_spot, out_stream, error);
    break;
})
DEF_OP(OR, false, "||", (NUMBER_1 || NUMBER_2), true,
{
    TranslateOrToAsm(tree, node, tables, &label_spot, out_stream, error);
    break;
})

DEF_OP(READ, false, "57>>", (0), false,
{
    fprintf(out_stream, "in\n");

    int ram_id = GetNameRamIdFromStack(tables, NODE_NAME(node->left));

    fprintf(out_stream, "pop [%d]\n", ram_id);
    break;
})

DEF_OP(PRINT, false, "57<<", (0), false,
{
    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);
    fprintf(out_stream, "out\n");
    break;
})

DEF_OP(TYPE_INT, false, "57::", (0), false, {})

DEF_OP(L_BRACKET, true, "(", (0), false, {})
DEF_OP(R_BRACKET, true, ")", (0), false, {})
DEF_OP(COMMA, true, ",", (0), false,
{
    TranslateNodeToAsm(tree, node->right, tables, out_stream, error);
    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);
    break;
})
DEF_OP(LINE_END, true, "\n", (0), false,
{
    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);
    TranslateNodeToAsm(tree, node->right, tables, out_stream, error);
    break;
})
DEF_OP(BLOCK_END, false, ".57", (0), false, {})
DEF_OP(FUNC_WALL, false, "##################################################", (0), false, {})

DEF_OP(RETURN, false, "~57", (0), false,
{
    TranslateNodeToAsm(tree, node->left, tables, out_stream, error);
    fprintf(out_stream, "ret\n");
    break;
})

DEF_OP(END, false, "@57@", (0), false, {})




