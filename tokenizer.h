#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "stack.h"

#define STR_TOK(STR, TOK, VAL)  strTokCmp (STR, TOK, VAL)
#define NAME_TOK(STR, TOK, VAL) nameTok   (STR, VAL)
#define NUM_TOK(STR, TOK, VAL)  numTok    (STR, VAL)

#define TOKEN(NAME, tr1, tr2) TOK_##NAME,

enum TOKENS_TYPES
{
    TOK_TYPE_NUM,
    TOK_TYPE_NAME,
    N_TOKENS_TYPES
};

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

void        Tokenize      (const char *str, Stack *stk);

const char *strTokCmp     (const char *str, const char *tok, TokenValue *val);
const char *numTok        (const char *str, TokenValue *val);
const char *nameTok       (const char *str, TokenValue *val);

#endif  // TOKENIZER_H

