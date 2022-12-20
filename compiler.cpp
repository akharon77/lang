#include <stdio.h>

#include "compiler.h"
#include "tree.h"
#include "dsl.h"
#include "iostr.h"

const char    *STD_ASM_PATH  = "std.asm";
const int32_t  GLOB_SEC_PTR  = 0x12c;
const int32_t  INF           = 1e9;
const int32_t  NO_VAR        = -1;

#define CURR node

void CompilerInfoCtor(CompilerInfo *info)
{
    StackCtor(&info->fun_table, 0, sizeof(FunctionInfo));
    StackCtor(&info->globsp,    0, sizeof(char*));
    StackCtor(&info->namesp,    0, sizeof(NameTable));
    info->if_cnt = info->loop_cnt = 0;
}

void CompilerInfoDtor(CompilerInfo *info)
{
    StackDtor(&info->fun_table);
    StackDtor(&info->globsp);
    StackDtor(&info->namesp);
    info->if_cnt = info->loop_cnt = 0;
}

void CompileProgram(TreeNode *node, CompilerInfo *info, int32_t fd)
{
    Compile(node, info, fd, 0);
    AddStdAsmLib(fd);
    dprintf(fd, "push 900\n"
                "pop rgm\n"
                "push 1800\n"
                "pop rvb\n"
                "call main\n"
                "hlt\n");
}

void Compile(TreeNode *node, CompilerInfo *info, int32_t fd, int32_t depth)
{
    if (node == NULL)
        return;

    #define TYPE(name)                              \
        case TREE_NODE_TYPE_##name:                 \
            COMPILE_##name(CURR, info, fd, depth);         \
            break;

    switch (GET_TYPE(CURR))
    {
        #include "tree_node_types.h"
    }
    #undef TYPE
}

void COMPILE_DEFS(TreeNode *node, CompilerInfo *info, int32_t fd, int32_t depth)
{
    Compile(LEFT,  info, fd, depth);
    Compile(RIGHT, info, fd, depth);
}

void COMPILE_CONST(TreeNode *node, CompilerInfo *info, int32_t fd, int32_t depth)
{
    Tabstabe(fd, depth);
    dprintf(fd, "push %ld\n", (int64_t) (GET_NUM(CURR) * 1000));
}

void COMPILE_VAR(TreeNode *node, CompilerInfo *info, int32_t fd, int32_t depth)
{
    Tabstabe(fd, depth);
    dprintf(fd, "; var %s\n", GET_VAR(CURR));
    Tabstabe(fd, depth);
    dprintf(fd, "push ");
    PrintVarPointer(GET_VAR(CURR), info, fd);
    dprintf(fd, "\n");
}

void COMPILE_OP(TreeNode *node, CompilerInfo *info, int32_t fd, int32_t depth)
{
    Compile(LEFT,  info, fd, depth);
    Compile(RIGHT, info, fd, depth);

    #define TYPE(op_code, str, cmd)                        \
        case OP_TYPE_##op_code:                            \
            Tabstabe(fd, depth);                           \
            dprintf(fd, "; evaluation of " #op_code "\n"); \
            Tabstabe(fd, depth);                           \
            dprintf(fd, "%s\n", cmd);                      \
            break;

    switch (GET_OP(CURR))
    {
        #include "op_types.h"
    }
    #undef TYPE
}

void COMPILE_NVAR(TreeNode *node, CompilerInfo *info, int32_t fd, int32_t depth)
{
    int32_t ptr = 0;

    Tabstabe(fd, depth);
    dprintf(fd, "; new var %s\n", GET_VAR(CURR));
    Compile(RIGHT, info, fd, depth);

    if (info->namesp.size == 0)
    {
        ptr = AddGlobalVar(&info->globsp, GET_VAR(CURR));
        Tabstabe(fd, depth);
        dprintf(fd, "pop [%d+rgm]\n", ptr);
    }
    else
    {
        ptr = AddLocalVar(&info->namesp, GET_VAR(CURR));
        Tabstabe(fd, depth);
        dprintf(fd, "pop [%d+rbp]\n", ptr);
    }
}

void COMPILE_NFUN(TreeNode *node, CompilerInfo *info, int32_t fd, int32_t depth)
{
    Tabstabe(fd, depth);
    dprintf(fd, "; begin function %s\n", GET_VAR(CURR));
    Tabstabe(fd, depth);
    dprintf(fd, "jmp %s_end\n", GET_VAR(CURR));
    Tabstabe(fd, depth);
    dprintf(fd, "%s:\n", GET_VAR(CURR));

    FunctionInfo func = 
        {
            .name = strdup(GET_VAR(CURR)),
            .arg_cnt = 0
        };

    MakeNamespace(&info->namesp);

    TreeNode *par = LEFT;
    while (par != NULL)
    {
        Tabstabe(fd, depth);
        dprintf(fd, "; arg %s\n", GET_VAR(par));
        int32_t ptr = AddLocalVar(&info->namesp, GET_VAR(par));

        ++func.arg_cnt;
        par = par->right;
    }

    for (int32_t i = 0; i < func.arg_cnt; ++i)
    {
        Tabstabe(fd, depth);
        dprintf(fd, "pop [%d+rbp]\n", func.arg_cnt - i - 1);
    }

    StackPush(&info->fun_table, (void*) &func);

    Compile(RIGHT, info, fd, depth + 1);

    Tabstabe(fd, depth);
    dprintf(fd, "push 0\n");

    Tabstabe(fd, depth);
    dprintf(fd, "ret\n");

    Tabstabe(fd, depth);
    dprintf(fd, "%s_end:\n",
                GET_VAR(CURR));

    Tabstabe(fd, depth);
    dprintf(fd, "; end function %s\n", GET_VAR(CURR));
    
    CloseNamespace(&info->namesp);
}

void COMPILE_RET(TreeNode *node, CompilerInfo *info, int32_t fd, int32_t depth)
{
    Compile(RIGHT, info, fd, depth);
    Tabstabe(fd, depth);
    dprintf(fd, "ret\n");
}

void COMPILE_BLOCK(TreeNode *node, CompilerInfo *info, int32_t fd, int32_t depth)
{
    Tabstabe(fd, depth);
    dprintf(fd, "; new block\n");

    RepeatNameTable(&info->namesp);
    Compile(RIGHT, info, fd, depth + 1);
    CloseNamespace(&info->namesp);

    Tabstabe(fd, depth);
    dprintf(fd, "; end of block\n");
}

void COMPILE_SEQ(TreeNode *node, CompilerInfo *info, int32_t fd, int32_t depth)
{
    Compile(LEFT,  info, fd, depth);
    if (GET_TYPE(LEFT) == TREE_NODE_TYPE_CALL)
    {
        Tabstabe(fd, depth);
        dprintf(fd, "pop\n");
    }

    Compile(RIGHT, info, fd, depth);
}

void COMPILE_ASS(TreeNode *node, CompilerInfo *info, int32_t fd, int32_t depth)
{
    Tabstabe(fd, depth);
    dprintf(fd, "; var %s\n", GET_VAR(CURR));

    int32_t ptr = GetVarPointer(&info->namesp, GET_VAR(CURR));
    Compile(RIGHT, info, fd, depth + 1);
    Tabstabe(fd, depth);
    dprintf(fd, "pop ");
    PrintVarPointer(GET_VAR(CURR), info, fd);
    dprintf(fd, "\n");
}

void COMPILE_IF(TreeNode *node, CompilerInfo *info, int32_t fd, int32_t depth)
{
    Tabstabe(fd, depth);
    dprintf(fd, "; if\n");
    Compile(LEFT, info, fd, depth + 1);

    int32_t id = info->if_cnt++;
    Tabstabe(fd, depth);
    dprintf(fd, "pop rax\n");
    Tabstabe(fd, depth);
    dprintf(fd, "cmp rax 0\n");
    Tabstabe(fd, depth);
    dprintf(fd, "jne if%d\n", id);
    Tabstabe(fd, depth);
    dprintf(fd, "jmp else%d\n", id);
    Tabstabe(fd, depth);
    dprintf(fd, "if%d:\n", id);
    Compile(RIGHT->left, info, fd, depth + 1);

    Tabstabe(fd, depth);
    dprintf(fd, "jmp end_if%d\n", id);
    Tabstabe(fd, depth);
    dprintf(fd, "else%d:\n", id);
    Tabstabe(fd, depth);
    dprintf(fd, "; else\n");
    Compile(RIGHT->right, info, fd, depth + 1);

    Tabstabe(fd, depth);
    dprintf(fd, "end_if%d:\n", id);
    Tabstabe(fd, depth);
    dprintf(fd, "; end if\n");
}

void COMPILE_WHILE(TreeNode *node, CompilerInfo *info, int32_t fd, int32_t depth)
{
    Tabstabe(fd, depth);
    dprintf(fd, "; while\n");
    int32_t id = info->loop_cnt++;
    Tabstabe(fd, depth);
    dprintf(fd, "loop%d:\n", id);

    Compile(LEFT, info, fd, depth + 1);
    Tabstabe(fd, depth);
    dprintf(fd, "pop rax\n");
    Tabstabe(fd, depth);
    dprintf(fd, "cmp rax 0\n");
    Tabstabe(fd, depth);
    dprintf(fd, "je end_loop%d\n", id);
    Compile(RIGHT, info, fd, depth + 1);

    Tabstabe(fd, depth);
    dprintf(fd, "jmp loop%d\n", id);
    Tabstabe(fd, depth);
    dprintf(fd, "end_loop%d:\n", id);
    Tabstabe(fd, depth);
    dprintf(fd, "; end while\n");
}

void COMPILE_CALL(TreeNode *node, CompilerInfo *info, int32_t fd, int32_t depth)
{
    Tabstabe(fd, depth);
    dprintf(fd, "; fun %s call\n", GET_VAR(CURR));
    Compile(RIGHT, info, fd, depth + 1);

    int32_t free_ptr = ((NameTable*) StackGetPtr(&info->namesp, info->namesp.size - 1))->free_ptr + 1;
    Tabstabe(fd, depth);
    dprintf(fd, "push rbp\n");
    Tabstabe(fd, depth);
    dprintf(fd, "push %d\n", free_ptr);
    Tabstabe(fd, depth);
    dprintf(fd, "add\n");
    Tabstabe(fd, depth);
    dprintf(fd, "pop rbp\n");
    Tabstabe(fd, depth);
    dprintf(fd, "call %s\n", GET_VAR(CURR));
    Tabstabe(fd, depth);
    dprintf(fd, "push rbp\n");
    Tabstabe(fd, depth);
    dprintf(fd, "push %d\n", free_ptr);
    Tabstabe(fd, depth);
    dprintf(fd, "sub\n");
    Tabstabe(fd, depth);
    dprintf(fd, "pop rbp\n");
                // free_ptr,
                // GET_VAR(CURR),
                // free_ptr);
}

void COMPILE_PAR(TreeNode *node, CompilerInfo *info, int32_t fd, int32_t depth)
{
    Compile(LEFT,  info, fd, depth);
    Compile(RIGHT, info, fd, depth);
}

void COMPILE_BRANCH(TreeNode *node, CompilerInfo *info, int32_t fd, int32_t depth)
{

}

void COMPILE_ARG(TreeNode *node, CompilerInfo *info, int32_t fd, int32_t depth)
{

}

void PrintVarPointer(const char *name, CompilerInfo *info, int32_t fd)
{
    int32_t loc_var_ptr  = GetVarPointer     (&info->namesp, name);
    int32_t glob_var_ptr = GetGlobVarPointer (&info->globsp, name);

    if (loc_var_ptr != NO_VAR)
    {
        dprintf(fd, "[%d+rbp]", loc_var_ptr);
    }
    else if (glob_var_ptr != NO_VAR)
    {
        dprintf(fd, "[%d+rgm]", glob_var_ptr);
    }
    else
    {
        dprintf(2, "Var %s doesn't exist!\n", name);
        ASSERT(0);
    }
}

int32_t AddGlobalVar(Stack *stk, const char *name)
{
    char *new_name = strdup(name);
    StackPush(stk, &new_name);
    return stk->size - 1;
}

int32_t GetGlobVarPointer(Stack *stk, const char *name)
{
    for (int32_t i = 0; i < stk->size; ++i)
    {
        char **var_name = (char**) StackGetPtr(stk, i);
        
        if (strcmp(*var_name, name) == 0)
            return i;
    }

    return NO_VAR;
}

int32_t GetVarPointer(Stack *stk, const char *name)
{
    NameTable *name_table = (NameTable*) StackGetPtr(stk, stk->size - 1);
    Stack     *vars       = &name_table->stk;

    for (int32_t i = 0; i < vars->size; ++i)
    {
        LocalVar *var = (LocalVar*) StackGetPtr(vars, i);

        if (strcmp(var->name, name) == 0)
            return var->ptr;
    }

    return NO_VAR;
}

int32_t AddLocalVar(Stack *stk, const char *name)
{
    NameTable *name_table = (NameTable*) StackGetPtr(stk, stk->size - 1);
    Stack     *vars       = &name_table->stk;

    for (int32_t i = 0; i < vars->size; ++i)
    {
        LocalVar *var = (LocalVar*) StackGetPtr(vars, i);

        if (strcmp(var->name, name) == 0)
            return var->ptr = name_table->free_ptr++;
    }

    LocalVar var =
        {
            .name = strdup(name),
            .ptr  = name_table->free_ptr++
        };

    StackPush(&name_table->stk, &var);

    return var.ptr;
}

void RepeatNameTable(Stack *stk)
{
    if (stk->size > 0)
    {
        NameTable cp_table = {};
        StackCtor(&cp_table.stk, 0, sizeof(LocalVar));
        NameTable *last_name_table = (NameTable*) StackGetPtr(stk, stk->size - 1);
        cp_table.free_ptr = last_name_table->free_ptr;

        Stack *vars = &last_name_table->stk;
        for (int32_t i = 0; i < vars->size; ++i)
            StackPush(&cp_table.stk, (void*) StackGetPtr(vars, i));

        StackPush(stk, &cp_table);
    }
}

void MakeNamespace(Stack *stk)
{
    NameTable cp_table = {};
    StackCtor(&cp_table.stk, 0, sizeof(LocalVar));
    cp_table.free_ptr = 0;

    StackPush(stk, &cp_table);
}

void CloseNamespace(Stack *stk)
{
    StackPop(stk);
}

void PreCompileOp(TreeNode *node)
{
    if (LEFT)
        PreCompileOp(LEFT);

    if (RIGHT)
        PreCompileOp(RIGHT);

    if (GET_TYPE(CURR) == TREE_NODE_TYPE_OP)
    switch (GET_OP(CURR))
    {
        case OP_TYPE_EXP:
            TreeNodeCtor(node, TREE_NODE_TYPE_CALL,
                         {.var = "exp"},
                         NULL,
                         PAR(node->left, PAR(node->right, NULL)));
            break;
        case OP_TYPE_MOD:
            TreeNodeCtor(node, TREE_NODE_TYPE_CALL,
                         {.var = "mod"},
                         NULL,
                         PAR(node->left, PAR(node->right, NULL)));
            break;
        case OP_TYPE_LEQ:
            TreeNodeCtor(node, TREE_NODE_TYPE_CALL,
                         {.var = "leq"},
                         NULL,
                         PAR(node->left, PAR(node->right, NULL)));
            break;
        case OP_TYPE_GEQ:
            TreeNodeCtor(node, TREE_NODE_TYPE_CALL,
                         {.var = "geq"},
                         NULL,
                         PAR(node->left, PAR(node->right, NULL)));
            break;
        case OP_TYPE_LT:
            TreeNodeCtor(node, TREE_NODE_TYPE_CALL,
                         {.var = "lt"},
                         NULL,
                         PAR(node->left, PAR(node->right, NULL)));
            break;
        case OP_TYPE_GT:
            TreeNodeCtor(node, TREE_NODE_TYPE_CALL,
                         {.var = "gt"},
                         NULL,
                         PAR(node->left, PAR(node->right, NULL)));
            break;
        case OP_TYPE_EQ:
            TreeNodeCtor(node, TREE_NODE_TYPE_CALL,
                         {.var = "eq"},
                         NULL,
                         PAR(node->left, PAR(node->right, NULL)));
            break;
        case OP_TYPE_NEQ:
            TreeNodeCtor(node, TREE_NODE_TYPE_CALL,
                         {.var = "neq"},
                         NULL,
                         PAR(node->left, PAR(node->right, NULL)));
            break;
        case OP_TYPE_NOT:
            TreeNodeCtor(node, TREE_NODE_TYPE_CALL,
                         {.var = "not"},
                         NULL,
                         PAR(node->right, NULL));
            break;
        case OP_TYPE_NEG:
            TreeNodeCtor(node, TREE_NODE_TYPE_CALL,
                         {.var = "neg"},
                         NULL,
                         PAR(node->right, NULL));
            break;
        case OP_TYPE_OR:
            TreeNodeCtor(node, TREE_NODE_TYPE_CALL,
                         {.var = "or"},
                         NULL,
                         PAR(node->left, PAR(node->right, NULL)));
            break;
        case OP_TYPE_AND:
            TreeNodeCtor(node, TREE_NODE_TYPE_CALL,
                         {.var = "and"},
                         NULL,
                         PAR(node->left, PAR(node->right, NULL)));
            break;
    }
    
}

void AddStdAsmLib(int32_t fd)
{
    int32_t err = 0;
    TextInfo text = {};

    TextInfoCtor(&text);
    InputText(&text, STD_ASM_PATH, &err);

    dprintf(fd, "%s\n", text.base);

    TextInfoDtor(&text);
}

void Decompile(TreeNode *node, int32_t fd, int32_t depth)
{
    if (CURR == NULL)
        return;

    #define TYPE(name)                  \
        case TREE_NODE_TYPE_##name:     \
            DECOMPILE_##name(CURR, fd, depth); \
            break;

    switch (GET_TYPE(CURR))
    {
        #include "tree_node_types.h"
    }
    #undef TYPE
}

void DECOMPILE_DEFS(TreeNode *node, int32_t fd, int32_t depth)
{
    Decompile(LEFT,  fd, depth);
    Decompile(RIGHT, fd, depth);
}

void DECOMPILE_CONST(TreeNode *node, int32_t fd, int32_t depth)
{
    dprintf(fd, "%ld", (int64_t) GET_NUM(CURR));
}

void DECOMPILE_VAR(TreeNode *node, int32_t fd, int32_t depth)
{
    dprintf(fd, "%s", GET_VAR(CURR));
}

void DECOMPILE_OP(TreeNode *node, int32_t fd, int32_t depth)
{
    Decompile(LEFT,  fd, depth);

    #define TYPE(op_code, str, cmd)   \
        case OP_TYPE_##op_code:       \
            dprintf(fd, " %s ", str); \
            break;

    switch (GET_OP(CURR))
    {
        #include "op_types.h"
    }
    #undef TYPE

    Decompile(RIGHT, fd, depth);
}

void DECOMPILE_NVAR(TreeNode *node, int32_t fd, int32_t depth)
{
    Tabstabe(fd, depth);
    dprintf(fd, "var %s = ", GET_VAR(CURR));
    Decompile(RIGHT, fd, depth);
    dprintf(fd, ";\n");
}

void DECOMPILE_NFUN(TreeNode *node, int32_t fd, int32_t depth)
{
    Tabstabe(fd, depth);
    dprintf(fd, "fun %s(", GET_VAR(node));
    Decompile(LEFT, fd, depth);
    dprintf(fd, ")\n");

    Decompile(RIGHT, fd, depth);
}

void DECOMPILE_RET(TreeNode *node, int32_t fd, int32_t depth)
{
    Tabstabe(fd, depth);
    dprintf(fd, "ret ");
    Decompile(RIGHT, fd, depth);
    dprintf(fd, ";\n");
}

void DECOMPILE_BLOCK(TreeNode *node, int32_t fd, int32_t depth)
{
    Tabstabe(fd, depth);
    dprintf(fd, "{\n");
    Decompile(RIGHT, fd, depth + 1);
    Tabstabe(fd, depth);
    dprintf(fd, "}\n\n");
}

void DECOMPILE_SEQ(TreeNode *node, int32_t fd, int32_t depth)
{
    if (GET_TYPE(LEFT) == TREE_NODE_TYPE_CALL)
        Tabstabe(fd, depth);
    Decompile(LEFT,  fd, depth);
    if (GET_TYPE(LEFT) == TREE_NODE_TYPE_CALL)
        dprintf(fd, ";\n");
    Decompile(RIGHT, fd, depth);
}

void DECOMPILE_ASS(TreeNode *node, int32_t fd, int32_t depth)
{
    Tabstabe(fd, depth);
    dprintf(fd, "%s = ", GET_VAR(CURR));
    Decompile(RIGHT, fd, depth);
    dprintf(fd, ";\n");
}

void DECOMPILE_IF(TreeNode *node, int32_t fd, int32_t depth)
{
    Tabstabe(fd, depth);
    dprintf(fd, "if (");
    Decompile(LEFT, fd, depth);
    dprintf(fd, ")\n");

    Decompile(RIGHT->left, fd, depth + 1);

    if (RIGHT->right)
    {
        Tabstabe(fd, depth);
        dprintf(fd, "else\n");
        Decompile(RIGHT->right, fd, depth + 1);
    }
}

void DECOMPILE_WHILE(TreeNode *node, int32_t fd, int32_t depth)
{
    Tabstabe(fd, depth);
    dprintf(fd, "while (");
    Decompile(LEFT, fd, depth);
    dprintf(fd, ")\n");

    Decompile(RIGHT, fd, depth + 1);
}

void DECOMPILE_CALL(TreeNode *node, int32_t fd, int32_t depth)
{
    dprintf(fd, "%s(", GET_VAR(CURR));
    Decompile(RIGHT, fd, depth);
    dprintf(fd, ")");
}

void DECOMPILE_PAR(TreeNode *node, int32_t fd, int32_t depth)
{
    Decompile(LEFT,  fd, depth);
    if (RIGHT)
    {
        dprintf(fd, ", ");
        Decompile(RIGHT, fd, depth);
    }
}

void DECOMPILE_BRANCH(TreeNode *node, int32_t fd, int32_t depth)
{

}

void DECOMPILE_ARG(TreeNode *node, int32_t fd, int32_t depth)
{
    dprintf(fd, "%s", GET_VAR(node));
    if (RIGHT)
    {
        dprintf(fd, ", ");
        Decompile(RIGHT, fd, depth);
    }
}

void Tabstabe(int32_t fd, int32_t depth)
{
    for (int32_t i = 0; i < depth; ++i)
        dprintf(fd, "\t");
}

#undef CURR

