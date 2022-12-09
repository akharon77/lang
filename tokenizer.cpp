#include "lexer.h"

#define TOKEN(NAME, FUNC, STR)                          \
do                                                      \
{                                                       \
    const char *res = FUNC(str, STR);                   \
    if (res != str)                                     \
    {                                                   \
        StackPush(stk, TOK_##NAME, sizeof(int32_t));    \
        str = res;                                      \
    }                                                   \
} while (0)

void Tokenize(const char *str, Stack *stk)
{
    while (*str != '\0')
    {
        #include "lexems.h"
    }
}
#undef TOKEN

const char *strTokCmp(const char *str, const char *tok)
{
    int tok_len = strlen(tok);

    if (strncasecmp(str, tok, tok_len) == 0)
        return str + tok_len;
}

const char *nameTok(const char *str, const char *tok)
{
    
}
