#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "stack.h"

#define STR_CMP(STR, TOK, VAL)   strCmp   (STR, TOK, VAL)
#define IDENT_CMP(STR, TOK, VAL) identCmp (STR, VAL)
#define NUM_CMP(STR, TOK, VAL)   numCmp   (STR, VAL)

enum TOKENS_TYPES
{
    TOK_TYPE_CONST,
    TOK_TYPE_OP,
    TOK_TYPE_KEYWORD,
    TOK_TYPE_DELIM,
    TOK_TYPE_IDENT,
    N_TOKENS_TYPES
};

#define TOKEN(NAME, tr0, tr1, tr2) TOK_##NAME,

enum TOKENS
{
    #include "tokens.h"
    N_TOKENS
};
#undef TOKEN

union TokenValue
{
    double  num;
    char   *name;
};

struct Token
{
    int32_t    id;
    int32_t    type;

    TokenValue val;
};

void        TokenizerCtor (Stack *stk);
void        TokenizerDtor (Stack *stk);

void        TokenCtor     (Token *tok);
void        TokenDtor     (Token *tok);
void        TokenEmpty    (Token *tok);

void        Tokenize      (Stack *stk, const char *str);

bool        TestToken     (Stack *stk, int32_t id);
Token       NextToken     (Stack *stk);

const char *strCmp        (const char *str, const char *tok, Token *val);
const char *numCmp        (const char *str, Token *val);
const char *identCmp      (const char *str, Token *val);

#endif  // TOKENIZER_H

