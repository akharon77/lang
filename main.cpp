#include <stdio.h>

#include "tokenizer.h"
#include "stack.h"

int main()
{
    Stack stk = {};
    TokenizerCtor(&stk);

    Tokenize(&stk, "var x = (((x^2 + Y^2 + 6)-7)/   2);");
    for (int32_t i = 0; i < stk.size; ++i)
    {
        Token tok = {};
        StackGet(&stk, i, (void*) &tok);
        
        printf("id = %d, type = %d, ", tok.id, tok.type);
        if (tok.type == TOK_TYPE_NAME)
            printf("val = %s\n", tok.val.name);
        else
            printf("val = %g\n", tok.val.num);
    }

    return 0;
}
