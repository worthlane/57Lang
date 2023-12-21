#ifndef DEF_OP
#define DEF_OP(...) ;
#endif

// ======================================================================
// FORMAT
// ======================================================================

// DEF_OP(NAME, IS_CHAR, SYMBOLS, ACTION, IS_CALC)

// ======================================================================
// OPERATIONS
// ======================================================================

DEF_OP(ADD, true, "+", (NUMBER_1 + NUMBER_2), true)
DEF_OP(SUB, true, "-", (NUMBER_1 - NUMBER_2), true)
DEF_OP(MUL, true, "*", (NUMBER_1 * NUMBER_2), true)
DEF_OP(DIV, true, "/", (NUMBER_1 / NUMBER_2), true)
DEF_OP(DEG, true, "^", ( pow(NUMBER_1, NUMBER_2) ), true)

DEF_OP(ASSIGN, false, ":=", (0), false)
DEF_OP(SIN, false, "$1#", ( sin( NUMBER_1 )), true)
DEF_OP(COS, false, "<0$", ( cos( NUMBER_1 )), true)
DEF_OP(IF, false, "57?", (0), false)
DEF_OP(WHILE, false, "1000_7", (0), false)

DEF_OP(GREATER, false,">", (NUMBER_1 > NUMBER_2), true)
DEF_OP(GREATER_EQ, false, ">=", (NUMBER_1 >= NUMBER_2), true)
DEF_OP(LESS, false, "<", (NUMBER_1 < NUMBER_2), true)
DEF_OP(LESS_EQ, false, "<=", (NUMBER_1 <= NUMBER_2), true)
DEF_OP(EQ,false, "==", (NUMBER_1 == NUMBER_2), true)
DEF_OP(NOT_EQ, false, "!=", (NUMBER_1 != NUMBER_2), true)
DEF_OP(AND, false, "&&", (NUMBER_1 && NUMBER_2), true)
DEF_OP(OR, false, "||", (NUMBER_1 || NUMBER_2), true)

DEF_OP(READ, false, "57>>", (0), false)
DEF_OP(PRINT, false, "57<<", (0), false)

DEF_OP(TYPE_INT, false, "57::", (0), false)

DEF_OP(L_BRACKET, true, "(", (0), false)
DEF_OP(R_BRACKET, true, ")", (0), false)
DEF_OP(COMMA, true, ",", (0), false)
DEF_OP(LINE_END, true, "\n", (0), false)
DEF_OP(BLOCK_END, false, ".57", (0), false)
DEF_OP(FUNC_WALL, false, "##################################################", (0), false)

DEF_OP(RETURN, false, "~57", (0), false)

DEF_OP(END, false, "@57@", (0), false)




