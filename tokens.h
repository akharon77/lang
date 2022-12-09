TOKEN(NUM,      CONST,   NUM_CMP, "")

TOKEN(PLUS,     OP,      STR_CMP, "+")
TOKEN(MINUS,    OP,      STR_CMP, "-")
TOKEN(MUL,      OP,      STR_CMP, "*")
TOKEN(DIV,      OP,      STR_CMP, "/")
TOKEN(EXP,      OP,      STR_CMP, "^")
TOKEN(ASS,      OP,      STR_CMP, "=")

TOKEN(NVAR,     KEYWORD, STR_CMP, "var")
TOKEN(IF,       KEYWORD, STR_CMP, "if")
TOKEN(WHILE,    KEYWORD, STR_CMP, "while")

TOKEN(L_FIG_BR, DELIM,   STR_CMP, "{")
TOKEN(R_FIG_BR, DELIM,   STR_CMP, "}")
TOKEN(L_RND_BR, DELIM,   STR_CMP, "(")
TOKEN(R_RND_BR, DELIM,   STR_CMP, ")")
TOKEN(COMMA,    DELIM,   STR_CMP, ",")
TOKEN(DELIM,    DELIM,   STR_CMP, ";")

TOKEN(NAME,     IDENT,   IDENT_CMP, "")

