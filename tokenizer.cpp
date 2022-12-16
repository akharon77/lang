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
        .type = TOK_TYPE_IDENT,
        .val  = 
        {
            .name =  NULL
        }
    };
}

void TokenDtor(Token *tok)
{
    if (tok->type != TOK_TYPE_CONST)
        free(tok->val.name);

    TokenEmpty(tok);
}

#define TOKEN(NAME, TYPE, CMP, STR, OP_CODE, CMD)       \
{                                                       \
    Token res_tok = {};                                 \
    TokenCtor(&res_tok);                                \
                                                        \
    const char *res = CMP(str, STR, &res_tok);          \
    if (res != str)                                     \
    {                                                   \
        res_tok.id = TOK_##NAME;                        \
        res_tok.type = TOK_TYPE_##TYPE;                 \
        StackPush(&buf_stk, &res_tok);                  \
        str = res;                                      \
        continue;                                       \
    }                                                   \
}

void Tokenize(Stack *stk, const char *str)
{
    Stack buf_stk = {};
    StackCtor(&buf_stk, 0, sizeof(Token));

    while (*str != '\0')
    {
        if (isspace(*str))
        {
            ++str;
            continue;
        }

        #include "tokens.h"
    }

    while (buf_stk.size > 0)
    {
        Token tok = {};
        StackPop(&buf_stk, &tok);
        StackPush(stk, &tok);
    }

    StackDtor(&buf_stk);
}
#undef TOKEN

bool TestToken(Stack *stk, int32_t id)
{
    if (stk->size == 0)
        return false;

    Token tok = {};
    StackTop(stk, &tok);

    return tok.id == id; 
}

Token NextToken(Stack *stk)
{
    Token tok = {};
    StackPop(stk, &tok);

    return tok;
}

const char *strCmp(const char *str, const char *tok, Token *val)
{
    int tok_len = strlen(tok);

    if (strncasecmp(str, tok, tok_len) == 0)
    {
        val->val  = {.name = strdup(tok)};
        return str + tok_len;
    }

    return str;
}

const char *numCmp(const char *str, Token *val)
{
    int64_t res    = 0;
    int32_t offset = 0;

    if (sscanf(str, "%ld%n", &res, &offset))
    {
        val->val  = {.num = res};
        return str + offset;
    }

    return str;
}

const char *identCmp(const char *str, Token *val)
{
    const char *str_old = str;

    while (*str != '\0' && (*str == '_' || !ispunct(*str)) && !isspace(*str))
        ++str;

    val->val  = {.name = strndup(str_old, str - str_old)};

    return str;
}

