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
    Compile(node, info, fd);
    AddStdAsmLib(fd);
    dprintf(fd, "call main\n"
                "hlt\n");
}

void Compile(TreeNode *node, CompilerInfo *info, int32_t fd)
{
    if (node == NULL)
        return;

    #define TYPE(name)                      \
        case TREE_NODE_TYPE_##name:         \
            COMPILE_##name(CURR, info, fd); \
            break;

    switch (GET_TYPE(CURR))
    {
        #include "tree_node_types.h"
    }
    #undef TYPE
}

void COMPILE_DEFS(TreeNode *node, CompilerInfo *info, int32_t fd)
{
    Compile(LEFT,  info, fd);
    Compile(RIGHT, info, fd);
}

void COMPILE_CONST(TreeNode *node, CompilerInfo *info, int32_t fd)
{
    dprintf(fd, "push %ld\n", (int64_t) (GET_NUM(CURR) * 1000));
}

void COMPILE_VAR(TreeNode *node, CompilerInfo *info, int32_t fd)
{
    dprintf(fd, "push ");
    PrintVarPointer(GET_VAR(CURR), info, fd);
    dprintf(fd, "\n");
}

void COMPILE_OP(TreeNode *node, CompilerInfo *info, int32_t fd)
{
    Compile(LEFT,  info, fd);
    Compile(RIGHT, info, fd);

    #define TYPE(op_code, str, cmd)   \
        case OP_TYPE_##op_code:       \
            dprintf(fd, "%s\n", cmd); \
            break;

    switch (GET_OP(CURR))
    {
        #include "op_types.h"
    }
    #undef TYPE
}

void COMPILE_NVAR(TreeNode *node, CompilerInfo *info, int32_t fd)
{
    int32_t ptr = 0;

    Compile(RIGHT, info, fd);

    if (info->namesp.size == 0)
    {
        ptr = AddGlobalVar(&info->globsp, GET_VAR(CURR));
        dprintf(fd, "pop [%d]\n", ptr);
    }
    else
    {
        ptr = AddLocalVar(&info->namesp, GET_VAR(CURR));
        dprintf(fd, "pop [%d+rbp]\n", ptr);
    }
}

void COMPILE_NFUN(TreeNode *node, CompilerInfo *info, int32_t fd)
{
    dprintf(fd, "jmp %s_end\n", GET_VAR(CURR));
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
        int32_t ptr = AddLocalVar(&info->namesp, GET_VAR(par));
        // dprintf(fd, "pop [%d+rbp]\n", ptr);

        ++func.arg_cnt;
        par = par->right;
    }

    for (int32_t i = 0; i < func.arg_cnt; ++i)
        dprintf(fd, "pop [%d+rbp]\n", func.arg_cnt - i - 1);

    StackPush(&info->fun_table, (void*) &func);

    Compile(RIGHT, info, fd);
    dprintf(fd, "push 0\n"
                "ret\n"
                "%s_end:\n",
                GET_VAR(CURR));
    
    CloseNamespace(&info->namesp);
}

void COMPILE_RET(TreeNode *node, CompilerInfo *info, int32_t fd)
{
    Compile(RIGHT, info, fd);
    dprintf(fd, "ret\n");
}

void COMPILE_BLOCK(TreeNode *node, CompilerInfo *info, int32_t fd)
{
    RepeatNameTable(&info->namesp);
    Compile(RIGHT, info, fd);
    CloseNamespace(&info->namesp);
}

void COMPILE_SEQ(TreeNode *node, CompilerInfo *info, int32_t fd)
{
    Compile(LEFT,  info, fd);
    if (GET_TYPE(LEFT) == TREE_NODE_TYPE_CALL)
        dprintf(fd, "pop\n");

    Compile(RIGHT, info, fd);
}

void COMPILE_ASS(TreeNode *node, CompilerInfo *info, int32_t fd)
{
    int32_t ptr = GetVarPointer(&info->namesp, GET_VAR(CURR));
    Compile(RIGHT, info, fd);
    dprintf(fd, "pop ");
    PrintVarPointer(GET_VAR(CURR), info, fd);
    dprintf(fd, "\n");
}

void COMPILE_IF(TreeNode *node, CompilerInfo *info, int32_t fd)
{
    Compile(LEFT, info, fd);

    int32_t id = info->if_cnt++;
    dprintf(fd, "pop rax\n"
                "cmp rax 0\n"
                "jne if%d\n"
                "jmp else%d\n"
                "if%d:\n", 
                id, id, id);
    Compile(RIGHT->left, info, fd);

    dprintf(fd, "jmp end_if%d\n"
                "else%d:\n",
                id, id);
    Compile(RIGHT->right, info, fd);

    dprintf(fd, "end_if%d:\n", id);
}

void COMPILE_WHILE(TreeNode *node, CompilerInfo *info, int32_t fd)
{
    int32_t id = info->loop_cnt++;
    dprintf(fd, "loop%d:\n", id);

    Compile(LEFT, info, fd);
    dprintf(fd, "pop rax\n"
                "cmp rax 0\n"
                "je end_loop%d\n", id);
    Compile(RIGHT, info, fd);

    dprintf(fd, "jmp loop%d\n"
                "end_loop%d:\n", id, id);
}

void COMPILE_CALL(TreeNode *node, CompilerInfo *info, int32_t fd)
{
    Compile(RIGHT, info, fd);

    int32_t free_ptr = ((NameTable*) StackGetPtr(&info->namesp, info->namesp.size - 1))->free_ptr + 1;
    dprintf(fd, "push rbp\n"
                "push %d\n"
                "add\n"
                "pop rbp\n" 
                "call %s\n"
                "push rbp\n"
                "push %d\n"
                "sub\n"
                "pop rbp\n",
                free_ptr,
                GET_VAR(CURR),
                free_ptr);
}

void COMPILE_PAR(TreeNode *node, CompilerInfo *info, int32_t fd)
{
    Compile(LEFT,  info, fd);
    Compile(RIGHT, info, fd);
}

void COMPILE_BRANCH(TreeNode *node, CompilerInfo *info, int32_t fd)
{

}

void COMPILE_ARG(TreeNode *node, CompilerInfo *info, int32_t fd)
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
        dprintf(fd, "[%d]", glob_var_ptr);
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
    return GLOB_SEC_PTR + stk->size - 1;
}

int32_t GetGlobVarPointer(Stack *stk, const char *name)
{
    for (int32_t i = 0; i < stk->size; ++i)
    {
        char **var_name = (char**) StackGetPtr(stk, i);
        
        if (strcmp(*var_name, name) == 0)
            return GLOB_SEC_PTR + i;
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

void Decompile(TreeNode *node, int32_t fd)
{
    if (CURR == NULL)
        return;

    #define TYPE(name)                  \
        case TREE_NODE_TYPE_##name:     \
            DECOMPILE_##name(CURR, fd); \
            break;

    switch (GET_TYPE(CURR))
    {
        #include "tree_node_types.h"
    }
    #undef TYPE
}

void DECOMPILE_DEFS(TreeNode *node, int32_t fd)
{
    Decompile(LEFT,  fd);
    Decompile(RIGHT, fd);
}

void DECOMPILE_CONST(TreeNode *node, int32_t fd)
{
    dprintf(fd, "%ld", (int64_t) GET_NUM(CURR));
}

void DECOMPILE_VAR(TreeNode *node, int32_t fd)
{
    dprintf(fd, "%s", GET_VAR(CURR));
}

void DECOMPILE_OP(TreeNode *node, int32_t fd)
{
    Decompile(LEFT,  fd);

    #define TYPE(op_code, str, cmd)   \
        case OP_TYPE_##op_code:       \
            dprintf(fd, " %s ", str); \
            break;

    switch (GET_OP(CURR))
    {
        #include "op_types.h"
    }
    #undef TYPE

    Decompile(RIGHT, fd);
}

void DECOMPILE_NVAR(TreeNode *node, int32_t fd)
{
    dprintf(fd, "var %s = ", GET_VAR(CURR));
    Decompile(RIGHT, fd);
    dprintf(fd, ";\n");
}

void DECOMPILE_NFUN(TreeNode *node, int32_t fd)
{
    dprintf(fd, "fun %s(", GET_VAR(node));
    Decompile(LEFT, fd);
    dprintf(fd, ")\n");

    Decompile(RIGHT, fd);
}

void DECOMPILE_RET(TreeNode *node, int32_t fd)
{
    dprintf(fd, "ret ");
    Decompile(RIGHT, fd);
    dprintf(fd, ";\n");
}

void DECOMPILE_BLOCK(TreeNode *node, int32_t fd)
{
    dprintf(fd, "{\n");
    Decompile(RIGHT, fd);
    dprintf(fd, "}\n");
}

void DECOMPILE_SEQ(TreeNode *node, int32_t fd)
{
    Decompile(LEFT,  fd);
    if (GET_TYPE(LEFT) == TREE_NODE_TYPE_CALL)
        dprintf(fd, ";\n");
    Decompile(RIGHT, fd);
}

void DECOMPILE_ASS(TreeNode *node, int32_t fd)
{
    dprintf(fd, "%s = ", GET_VAR(CURR));
    Decompile(RIGHT, fd);
    dprintf(fd, ";\n");
}

void DECOMPILE_IF(TreeNode *node, int32_t fd)
{
    dprintf(fd, "if (");
    Decompile(LEFT, fd);
    dprintf(fd, ")\n");

    Decompile(RIGHT->left, fd);

    if (RIGHT->right)
    {
        dprintf(fd, "else\n");
        Decompile(RIGHT->right, fd);
    }
}

void DECOMPILE_WHILE(TreeNode *node, int32_t fd)
{
    dprintf(fd, "while (");
    Decompile(LEFT, fd);
    dprintf(fd, ")\n");

    Decompile(RIGHT, fd);
}

void DECOMPILE_CALL(TreeNode *node, int32_t fd)
{
    dprintf(fd, "%s(", GET_VAR(CURR));
    Decompile(RIGHT, fd);
    dprintf(fd, ")");
}

void DECOMPILE_PAR(TreeNode *node, int32_t fd)
{
    Decompile(LEFT,  fd);
    if (RIGHT)
    {
        dprintf(fd, ", ");
        Decompile(RIGHT, fd);
    }
}

void DECOMPILE_BRANCH(TreeNode *node, int32_t fd)
{

}

void DECOMPILE_ARG(TreeNode *node, int32_t fd)
{
    dprintf(fd, "%s", GET_VAR(node));
    if (RIGHT)
    {
        dprintf(fd, ", ");
        Decompile(RIGHT, fd);
    }
}

#undef CURR

