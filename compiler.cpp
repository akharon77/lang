#include <stdio.h>

#include "compiler.h"
#include "tree.h"
#include "dsl.h"

#define CURR node

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

void COMPILE_NUM(TreeNode *node, CompilerInfo *info, int32_t fd)
{
    dprintf(fd, "push %ld\n", GET_NUM(CURR));
}

void COMPILE_VAR(TreeNode *node, CompilerInfo *info, int32_t fd)
{
    dprintf(fd, "push [%d+rbp]\n", GetVarPointer(&info->name_table, GET_VAR(CURR)));
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
    int32_t ptr = AddLocalVar(&info->name_table, GET_VAR(node));
    Compile(RIGHT, info, fd);
    dprintf(fd, "pop [%d+rbp]\n", ptr);
}

void COMPILE_NFUN(TreeNode *node, CompilerInfo *info, int32_t fd)
{
    dprintf(fd, "%s:\n", GET_VAR(node));

    FunctionInfo func = 
        {
            .name = strdup(GET_VAR(CURR)),
            .arg_cnt = 0
        };

    TreeNode *par = LEFT;
    while (par != NULL)
    {
        int32_t ptr = AddLocalVar(&info->name_table, GET_VAR(par));
        dprintf(fd, "pop [%d+rbp]\n", ptr);

        ++func.arg_cnt;
        par = par->right;
    }

    StackPush(&info->fun_table, (void*) &func);

    Compile(RIGHT->right, info, fd);
}

void COMPILE_RET(TreeNode *node, CompilerInfo *info, int32_t fd)
{
    Compile(RIGHT, info, fd);
    dprintf(fd, "ret\n");
}

void COMPILE_BLOCK(TreeNode *node, CompilerInfo *info, int32_t fd)
{
    MakeNamespace(&info->name_table);
    Compile(RIGHT, info, fd);
    CloseNamespace(&info->name_table);
}

void COMPILE_SEQ(TreeNode *node, CompilerInfo *info, int32_t fd)
{
    Compile(LEFT,  info, fd);
    Compile(RIGHT, info, fd);
}

void COMPILE_ASS(TreeNode *node, CompilerInfo *info, int32_t fd)
{
    int32_t ptr = GetVarPointer(&info->name_table, GET_VAR(CURR));
    Compile(RIGHT, info, fd);
    dprintf(fd, "pop [%d+rbp]\n", ptr);
}

void COMPILE_IF(TreeNode *node, CompilerInfo *info, int32_t fd)
{
    Compile(LEFT, info, fd);

    dprintf(fd, "pop rax\n"
                "cmp rax 0\n"
                "ja  if%d\n"
                "jmp else%d\n"
                "if%d:\n", 
                info->if_cnt, info->if_cnt, info->if_cnt);
    Compile(RIGHT->left, info, fd);

    dprintf(fd, "jmp end_if%d\n"
                "else%d:\n",
                info->if_cnt, info->if_cnt);
    Compile(RIGHT->right, info, fd);

    dprintf(fd, "end_if%d:\n", info->if_cnt);
    ++info->if_cnt;
}

void COMPILE_WHILE(TreeNode *node, CompilerInfo *info, int32_t fd)
{
    dprintf(fd, "loop%d:\n", info->loop_cnt);

    Compile(LEFT, info, fd);
    dprintf(fd, "pop rax\n"
                "cmp rax 0\n"
                "jbe end_loop%d\n", info->loop_cnt);
    Compile(RIGHT, info, fd);

    dprintf(fd, "jmp loop%d\n"
                "end_loop%d\n", info->loop_cnt, info->loop_cnt);
    ++info->loop_cnt;
}

void COMPILE_CALL(TreeNode *node, CompilerInfo *info, int32_t fd)
{
    Compile(RIGHT, info, fd);
    dprintf(fd, "push rbp\n"
                "push %d\n"
                "add\n"
                "pop rbp\n" 
                "call %s\n"
                "push rbp\n"
                "push %d\n"
                "sub\n"
                "pop rbp\n",
                info->name_table.free_ptr,
                GET_VAR(CURR),
                info->name_table.free_ptr);
}

void COMPILE_ARG(TreeNode *node, CompilerInfo *info, int32_t fd)
{
    Compile(LEFT,  info, fd);
    Compile(RIGHT, info, fd);
}

void COMPILE_BRANCH(TreeNode *node, CompilerInfo *info, int32_t fd)
{

}

void COMPILE_PAR(TreeNode *node, CompilerInfo *info, int32_t fd)
{

}

int32_t GetVarPointer(NameTable *name_table, const char *name)
{
    for (int32_t i = 0; i < name_table->stk.size; ++i)
    {
        LocalVar *var = (LocalVar*) StackGetPtr(&name_table->stk, i);

        if (strcasecmp(var->name, name) == 0)
            return *((int32_t*) StackGetPtr(&var->mem, var->mem.size - 1));
    }

    assert(0 && "No var\n");  // TODO: fix CE
}

int32_t AddLocalVar(NameTable *name_table, const char *name)
{
    for (int32_t i = 0; i < name_table->stk.size; ++i)
    {
        LocalVar *var = (LocalVar*) StackGetPtr(&name_table->stk, i);

        if (strcasecmp(var->name, name) == 0)
        {
            *((int32_t*) StackGetPtr(&var->mem, var->mem.size - 1)) = name_table->free_ptr;
            return name_table->free_ptr++;
        }
    }

    LocalVar new_local_var = 
        {
            .name = strdup(name),
            .mem  = {}
        };

    StackCtor(&new_local_var.mem, 1, sizeof(int32_t));
    StackPush(&new_local_var.mem, &name_table->free_ptr);
    StackPush(&name_table->stk, &new_local_var);

    return name_table->free_ptr++;
}

void MakeNamespace(NameTable *name_table)
{
    for (int32_t i = 0; i < name_table->stk.size; ++i)
    {
        LocalVar *var = (LocalVar*) StackGetPtr(&name_table->stk, i);
        StackPush(&var->mem, StackGetPtr(&var->mem, var->mem.size - 1));
    }
}

void CloseNamespace(NameTable *name_table)
{
    int32_t min_free_ptr = INF;
    for (int32_t i = 0; i < name_table->stk.size; ++i)
    {
        LocalVar *var = (LocalVar*) StackGetPtr(&name_table->stk, i);

        if (var->mem.size > 0)
        {
            int32_t  *var_ptr = (int32_t*) StackGetPtr(&var->mem, var->mem.size - 1);

            if (*var_ptr < min_free_ptr)
                min_free_ptr = *var_ptr;

            StackPop(&var->mem);
        }
    }

    name_table->free_ptr = min_free_ptr;
}

#undef CURR
