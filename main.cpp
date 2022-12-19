#include <stdio.h>

#include "tokenizer.h"
#include "stack.h"
#include "parser.h"
#include "compiler.h"
#include "tree_debug.h"
#include "iostr.h"

int main(int argc, const char *argv[])
{
    int32_t err = 0;

    Stack stk = {};
    TokenizerCtor(&stk);

    TextInfo text = {};
    TextInfoCtor(&text);
    InputText(&text, argv[1], &err);

    Tokenize(&stk, text.base);

    // for (int32_t i = 0; i < stk.size; ++i)
    // {
    //     Token tok = {};
    //     StackGet(&stk, i, (void*) &tok);
    //     
    //     printf("id = %d, type = %d, ", tok.id, tok.type);
    //     if (tok.type == TOK_TYPE_CONST)
    //         printf("val = %g\n", tok.val.num);
    //     else
    //         printf("val = %s\n", tok.val.name);
    // }

    TreeNode *stmnt = TreeNodeNew();
    GetDefinitionStatementsList(&stk, stmnt);

    CompilerInfo info = {};

    StackCtor(&info.fun_table, 0, sizeof(FunctionInfo));
    FunctionInfo std_funcs[] = 
        {
            {"exp", 2},
            {"mod", 2},
            {"leq", 2},
            {"geq", 2},
            {"les", 2},
            {"ger", 2},
            {"eq",  2},
            {"neq", 2},
            {"not", 1},
            {"or",  2},
            {"and", 2},
            {"out", 1}
        };
    for (int32_t i = 0; i < 11; ++i)
        StackPush(&info.fun_table, (void*) &std_funcs[i]);

    StackCtor(&info.globsp, 0, sizeof(char*));
    StackCtor(&info.namesp, 0, sizeof(NameTable));
    info.if_cnt = info.loop_cnt = 0;

    // GetTree("{ DEFS, NULL, { NFUN, factorial, { PAR, cnt, {}, {} }, { BLOCK, NULL, {}, { SEQ, NULL, { IF, NULL, { OP, LT, { VAR, cnt, {}, {} }, { CONST, 0.000, {}, {} } }, { BRANCH, NULL, { RET, NULL, {}, { OP, NEG, {}, { CONST, 1000.000, {}, {} } } }, {} } }, { SEQ, NULL, { IF, NULL, { OP, EQ, { VAR, cnt, {}, {} }, { CONST, 0.000, {}, {} } }, { BRANCH, NULL, { RET, NULL, {}, { CONST, 1.000, {}, {} } }, {} } }, { SEQ, NULL, { RET, NULL, {}, { OP, MUL, { VAR, cnt, {}, {} }, { CALL, factorial, {}, { ARG, NULL, { OP, SUB, { VAR, cnt, {}, {} }, { CONST, 1.000, {}, {} } }, {} } } } }, {} } } } } }, { DEFS, NULL, { NFUN, main, {}, { BLOCK, NULL, {}, { SEQ, NULL, { NVAR, num, {}, { CALL, read, {}, {} } }, { SEQ, NULL, { NVAR, val, {}, { CALL, factorial, {}, { ARG, NULL, { VAR, num, {}, {} }, {} } } }, { SEQ, NULL, { CALL, print, {}, { ARG, NULL, { VAR, val, {}, {} }, {} } }, { SEQ, NULL, { RET, NULL, {}, { CONST, 0.000, {}, {} } }, {} } } } } } }, {} } }", stmnt);

    TreeDump(stmnt, "test");
    SaveToFile(stmnt, 1);

    PreCompileOp(stmnt);
    TreeDump(stmnt, "test");

    CompileProgram(stmnt, &info, 1);

    TextInfoDtor(&text);

    return 0;
}
