#ifndef DEF_OP
#define DEF_OP(...) ;
#endif

// ======================================================================
// FORMAT
// ======================================================================

// DEF_OP(NAME, IS_CHAR, SYMBOLS)

// ======================================================================
// OPERATIONS
// ======================================================================

DEF_OP(ADD, true, "+")

DEF_OP(SUB, true, "-")
DEF_OP(MUL, true, "*")
DEF_OP(DIV, true, "/")
DEF_OP(DEG, true, "^")

DEF_OP(ASSIGN, false, ":=")
DEF_OP(SIN, false, "$1#")
DEF_OP(COS, false, "<0$")
DEF_OP(IF, false, "57?")
DEF_OP(WHILE, false, "1000_7")

DEF_OP(GREATER, false,">")
DEF_OP(GREATER_EQ, false, ">=")
DEF_OP(LESS, false, "<")
DEF_OP(LESS_EQ, false, "<=")
DEF_OP(EQ,false, "==")
DEF_OP(NOT_EQ, false, "!=")
DEF_OP(AND, false, "&&")
DEF_OP(OR, false, "||")

DEF_OP(READ, false, "57>>")
DEF_OP(PRINT, false, "57<<")

DEF_OP(TYPE_INT, false, "57::")

DEF_OP(L_BRACKET, true, "(")
DEF_OP(R_BRACKET, true, ")")
DEF_OP(COMMA, true, ",")
DEF_OP(LINE_END, true, "\n")
DEF_OP(BLOCK_END, false, ".57")
DEF_OP(FUNC_WALL, false, "##################################################")

DEF_OP(RETURN, false, "~57")

DEF_OP(END, false, "@57@")




