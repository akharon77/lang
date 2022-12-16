#include <stdio.h>

#include "tokenizer.h"
#include "stack.h"
#include "parser.h"
#include "compiler.h"
#include "tree_debug.h"

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

    TreeNode *stmnt = TreeNodeNew();
    GetStatementsList(&stk, stmnt);

    TreeDump(stmnt, "test");

    CompilerInfo info = {};
    StackCtor(&info.fun_table, 0, sizeof(FunctionInfo));
    StackCtor(&info.name_table.stk, 0, sizeof(LocalVar));
    info.if_cnt = info.loop_cnt = 0;

    Compile(stmnt, &info, 1);

    return 0;
}
