TOKEN(PLUS,     OP,      STR_CMP, "+")
TOKEN(MINUS,    OP,      STR_CMP, "-")
TOKEN(NOT,      OP,      STR_CMP, "!")
TOKEN(MUL,      OP,      STR_CMP, "*")
TOKEN(DIV,      OP,      STR_CMP, "/")
TOKEN(EXP,      OP,      STR_CMP, "^")
TOKEN(PRCNT,    OP,      STR_CMP, "%")

TOKEN(LES,      OP,      STR_CMP, "<")
TOKEN(LEQ,      OP,      STR_CMP, "<=")
TOKEN(GER,      OP,      STR_CMP, ">")
TOKEN(GEQ,      OP,      STR_CMP, ">=")
TOKEN(EQ,       OP,      STR_CMP, "==")
TOKEN(NEQ,      OP,      STR_CMP, "!=")

TOKEN(OR,       OP,      STR_CMP, "or")
TOKEN(AND,      OP,      STR_CMP, "and")

TOKEN(INTRG,    OP,      STR_CMP, "?")
TOKEN(TWDTS,    OP,      STR_CMP, ":")

TOKEN(NVAR,     KEYWORD, STR_CMP, "var")
TOKEN(IF,       KEYWORD, STR_CMP, "if")
TOKEN(ELSE,     KEYWORD, STR_CMP, "else")
TOKEN(WHILE,    KEYWORD, STR_CMP, "while")

TOKEN(ASS,      OP,      STR_CMP, "=")

TOKEN(L_FIG_BR, DELIM,   STR_CMP, "{")
TOKEN(R_FIG_BR, DELIM,   STR_CMP, "}")
TOKEN(L_RND_BR, DELIM,   STR_CMP, "(")
TOKEN(R_RND_BR, DELIM,   STR_CMP, ")")
TOKEN(COMMA,    DELIM,   STR_CMP, ",")
TOKEN(COMDOT,   DELIM,   STR_CMP, ";")

TOKEN(CONST,    CONST,   NUM_CMP, "")
TOKEN(IDENT,    IDENT,   IDENT_CMP, "")

