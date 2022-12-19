TOKEN (PLUS,     OP,      STR_CMP,   "+",     ADD,  "add")
TOKEN (MINUS,    OP,      STR_CMP,   "-",     SUB,  "sub")
TOKEN (MUL,      OP,      STR_CMP,   "*",     MUL,  "push 1000\ndiv\nmul")
TOKEN (DIV,      OP,      STR_CMP,   "/",     DIV,  "push 1000\ndiv\ndiv")
TOKEN (EXP,      OP,      STR_CMP,   "^",     EXP,  "call exp")
TOKEN (PRCNT,    OP,      STR_CMP,   "%",     MOD,  "call mod")

TOKEN (LEQ,      OP,      STR_CMP,   "<=",    LEQ,  "call leq")
TOKEN (GEQ,      OP,      STR_CMP,   ">=",    GEQ,  "call geq")
TOKEN (LT,       OP,      STR_CMP,   "<",     LT,   "call lt")
TOKEN (GT,       OP,      STR_CMP,   ">",     GT,   "call gt")
TOKEN (EQ,       OP,      STR_CMP,   "==",    EQ,   "call eq")
TOKEN (NEQ,      OP,      STR_CMP,   "!=",    NEQ,  "call neq")
TOKEN (NOT,      OP,      STR_CMP,   "!",     NOT,  "call not")

TOKEN (OR,       OP,      STR_CMP,   "or",    OR,   "call or")
TOKEN (AND,      OP,      STR_CMP,   "and",   AND,  "call and")

TOKEN (NVAR,     KEYWORD, STR_CMP,   "var",   TR1,  "")
TOKEN (IF,       KEYWORD, STR_CMP,   "if",    TR2,  "")
TOKEN (ELSE,     KEYWORD, STR_CMP,   "else",  TR3,  "")
TOKEN (WHILE,    KEYWORD, STR_CMP,   "while", TR4,  "")
TOKEN (NFUN,     KEYWORD, STR_CMP,   "fun",   TR5,  "")
TOKEN (RET,      KEYWORD, STR_CMP,   "ret",   TR6,  "")

TOKEN (ASS,      OP,      STR_CMP,   "=",     TR7,  "")

TOKEN (INTRG,    DELIM,   STR_CMP,   "?",     TR16, "")
TOKEN (TWDTS,    DELIM,   STR_CMP,   ":",     TR17, "")
TOKEN (L_FIG_BR, DELIM,   STR_CMP,   "{",     TR8,  "")
TOKEN (R_FIG_BR, DELIM,   STR_CMP,   "}",     TR9,  "")
TOKEN (L_RND_BR, DELIM,   STR_CMP,   "(",     TR10, "")
TOKEN (R_RND_BR, DELIM,   STR_CMP,   ")",     TR11, "")
TOKEN (COMMA,    DELIM,   STR_CMP,   ",",     TR12, "")
TOKEN (COMDOT,   DELIM,   STR_CMP,   ";",     TR13, "")

TOKEN (CONST,    CONST,   NUM_CMP,   "",      TR14, "")
TOKEN (IDENT,    IDENT,   IDENT_CMP, "",      TR15, "")

