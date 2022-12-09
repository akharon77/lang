#include <stdio.h>

#include "tokenizer.h"
#include "stack.h"

int main(int argc, const char *argv[])
{
    Stack stk = {};
    TokenizerCtor(&stk);

    Tokenize(&stk, argv[1]);
    for (int32_t i = 0; i < stk.size; ++i)
    {
        Token tok = {};
        StackGet(&stk, i, (void*) &tok);
        
        printf("id = %d, type = %d, ", tok.id, tok.type);
        if (tok.type == TOK_TYPE_CONST)
            printf("val = %g\n", tok.val.num);
        else
            printf("val = %s\n", tok.val.name);
    }

    return 0;
}
