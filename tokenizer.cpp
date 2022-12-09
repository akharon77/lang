#include <stdio.h>
#include <ctype.h>

#include "tokenizer.h"

void TokenizerCtor(Stack *stk)
{
    StackCtor(stk, 0, sizeof(Token));
}

void TokenizerDtor(Stack *stk)
{
    StackDtor(stk);
}

void TokenCtor(Token *tok)
{
    TokenEmpty(tok);
}

void TokenEmpty(Token *tok)
{
    *tok = 
    {
        .id   = 0,
        .type = TOK_TYPE_NAME,
        .val  = 
        {
            .name =  NULL
        }
    };
}

void TokenDtor(Token *tok)
{
    if (tok->type == TOK_TYPE_NAME)
        free(tok->val.name);

    TokenEmpty(tok);
}

#define TOKEN(NAME, FUNC, STR)                          \
{                                                       \
    Token res_tok = {};                                 \
    TokenCtor(&res_tok);                                \
                                                        \
    const char *res = FUNC(str, STR, &res_tok.val);     \
    if (res != str)                                     \
    {                                                   \
        StackPush(stk, &res_tok);                       \
        str = res;                                      \
    }                                                   \
                                                        \
    TokenDtor(&res_tok);                                \
}

void Tokenize(const char *str, Stack *stk)
{
    while (*str != '\0')
    {
        #include "tokens.h"
    }
}
#undef TOKEN

const char *strTokCmp(const char *str, const char *tok, TokenValue *val)
{
    int tok_len = strlen(tok);

    if (strncasecmp(str, tok, tok_len) == 0)
    {
        *val = {.name = strdup(tok)};
        return str + tok_len;
    }
}

const char *numTok(const char *str, TokenValue *val)
{
    double  res    = 0;
    int32_t offset = 0;

    if (sscanf(str, "%g%n", &res, &offset))
    {
        *val = {.num = res};
        return str + offset;
    }
}

const char *nameTok(const char *str, TokenValue *val)
{
    const char *str_old = str;
    char res[256] = "";

    while (!ispunct(*str))
        ++str;

    *val = {.name = strndup(str_old, str - str_old)};

    return str;
}

