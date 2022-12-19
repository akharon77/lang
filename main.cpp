#include <stdio.h>

#include "tokenizer.h"
#include "stack.h"
#include "parser.h"
#include "compiler.h"
#include "tree_debug.h"
#include "iostr.h"
#include "lang.h"
#include "colors.h"

const Option EXEC_OPTIONS[] = 
    {
        {"--input-lang",        "-il",  INPUT_LANG_FILE_OPTION,       "lang from file (default: input.lang)"},
        {"--input-tree",        "-it",  INPUT_TREE_FILE_OPTION,       "tree from file (default: input.ast)"},
        {"--output-tree",      "-ot",   OUTPUT_TREE_FILE_OPTION,      "output to tree file (default: output.ast)"},
        {"--output-asm",       "-oa",   OUTPUT_ASM_FILE_OPTION,       "output to asm file (default: output.asm)"},
        {"--output-lang",      "-ol",   OUTPUT_LANG_FILE_OPTION,      "output to lang file (default: output.lang)"},
        {"--tree-lang",        "-tl",   TREE_TO_LANG_COMPILE_OPTION,  "tree to lang decompilation"},
        {"--lang-tree",        "-lt",   LANG_TO_TREE_COMPILE_OPTION,  "lang to tree compilation"},
        {"--lang-asm",         "-la",   LANG_TO_ASM_COMPILE_OPTION,   "lang to asm compilation"},
        {"--help",             "-h",    HELP_OPTION,                  "show help"}
    };
 
const size_t N_EXEC_OPTIONS = sizeof(EXEC_OPTIONS) / sizeof(Option);

int main(int argc, const char *argv[])
{
    const char *input_lang_filename  = "input.lang", 
               *input_tree_filename  = "input.ast",
               *output_asm_filename  = "output.asm",
               *output_tree_filename = "output.ast",
               *output_lang_filename = "output.lang";

    int err = 0;
    int options[N_EXEC_OPTIONS] = {};

    bool ok = GetOptions(argc, argv, options, EXEC_OPTIONS, N_EXEC_OPTIONS);
    if (!ok)
    {
        printf(RED "Wrong arguments\n" NORMAL);
        return 1;
    }

    if (options[HELP_OPTION])
    {
        for (int i = 0; i < N_EXEC_OPTIONS; ++i)
            printf("%20s %10s %s\n",
                    EXEC_OPTIONS[i].strFormLong,
                    EXEC_OPTIONS[i].strFormShort,
                    EXEC_OPTIONS[i].description);
        return 0;
    }

    if (options[INPUT_LANG_FILE_OPTION])
        input_lang_filename = argv[options[INPUT_LANG_FILE_OPTION] + 1];
    if (options[INPUT_TREE_FILE_OPTION])
        input_tree_filename = argv[options[INPUT_TREE_FILE_OPTION] + 1];
    if (options[OUTPUT_TREE_FILE_OPTION])
        output_tree_filename = argv[options[OUTPUT_TREE_FILE_OPTION] + 1];
    if (options[OUTPUT_ASM_FILE_OPTION])
        output_asm_filename = argv[options[OUTPUT_ASM_FILE_OPTION] + 1];
    if (options[OUTPUT_LANG_FILE_OPTION])
        output_lang_filename = argv[options[OUTPUT_LANG_FILE_OPTION] + 1];

    if (options[LANG_TO_TREE_COMPILE_OPTION] || options[LANG_TO_ASM_COMPILE_OPTION])
    {
        Stack stk = {};
        TokenizerCtor(&stk);

        TextInfo text = {};
        TextInfoCtor(&text);
        InputText(&text, input_lang_filename, &err);

        Tokenize(&stk, text.base);

        TreeNode *stmnt = TreeNodeNew();
        GetDefinitionStatementsList(&stk, stmnt);

        if (options[LANG_TO_TREE_COMPILE_OPTION])
        {
            int32_t fd = creat(output_tree_filename, S_IRWXU);
            SaveToFile(stmnt, fd);
            close(fd);
        }
        else
        {
            int32_t fd = creat(output_asm_filename, S_IRWXU);

            CompilerInfo info = {};
            CompilerInfoCtor(&info);

            PreCompileOp(stmnt);
            CompileProgram(stmnt, &info, fd);

            CompilerInfoDtor(&info);
            close(fd);
        }

        TokenizerDtor(&stk);
        TextInfoDtor(&text);
    }
    else if (options[TREE_TO_LANG_COMPILE_OPTION])
    {
        TextInfo text = {};
        TextInfoCtor(&text);
        InputText(&text, input_tree_filename, &err);

        TreeNode *root = TreeNodeNew();
        GetTree(text.base, root);

        int32_t fd = creat(output_lang_filename, S_IRWXU);
        Decompile(root, fd);

        close(fd);
        TextInfoDtor(&text);
    }


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

    return 0;
}
