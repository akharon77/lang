#ifndef LEXER_H
#define LEXER_H

#include "stack.h"

#define STR_TOK  strTokCmp
#define NAME_TOK nameTok

#define TOKEN(NAME, tr1, tr2) TOK_##NAME,

enum TOKENS
{
    #include "lexems.h"
    N_TOKENS
};
#undef TOKEN

struct TOKEN
{
    int32_t id;

    union
    {
        double  num;
        char   *name;
    } val;
};

const char *strTokCmp(const char *str, const char *tok);

#endif  // LEXER_H

