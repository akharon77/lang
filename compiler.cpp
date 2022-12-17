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
    dprintf(fd, "push %ld\n", (int64_t) (GET_NUM(CURR) * 1000));
    // dprintf(fd, "push 100\n"
    //             "push %ld\n"
    //             "mul\n",
    //             GET_NUM(CURR));
}

void COMPILE_VAR(TreeNode *node, CompilerInfo *info, int32_t fd)
{
    dprintf(fd, "push [%d+rbp]\n", GetVarPointer(&info->namesp, GET_VAR(CURR)));
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
    int32_t ptr = AddLocalVar(&info->namesp, GET_VAR(node));
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
                "ret\n");
    
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
    Compile(RIGHT, info, fd);
}

void COMPILE_ASS(TreeNode *node, CompilerInfo *info, int32_t fd)
{
    int32_t ptr = GetVarPointer(&info->namesp, GET_VAR(CURR));
    Compile(RIGHT, info, fd);
    dprintf(fd, "pop [%d+rbp]\n", ptr);
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

    dprintf(2, "Var %s doesn't exist!\n", name);
    assert(0);
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
                         ARG(node->left, ARG(node->right, NULL)));
            break;
        case OP_TYPE_MOD:
            TreeNodeCtor(node, TREE_NODE_TYPE_CALL,
                         {.var = "mod"},
                         NULL,
                         ARG(node->left, ARG(node->right, NULL)));
            break;
        case OP_TYPE_LEQ:
            TreeNodeCtor(node, TREE_NODE_TYPE_CALL,
                         {.var = "leq"},
                         NULL,
                         ARG(node->left, ARG(node->right, NULL)));
            break;
        case OP_TYPE_GEQ:
            TreeNodeCtor(node, TREE_NODE_TYPE_CALL,
                         {.var = "geq"},
                         NULL,
                         ARG(node->left, ARG(node->right, NULL)));
            break;
        case OP_TYPE_LES:
            TreeNodeCtor(node, TREE_NODE_TYPE_CALL,
                         {.var = "les"},
                         NULL,
                         ARG(node->left, ARG(node->right, NULL)));
            break;
        case OP_TYPE_GER:
            TreeNodeCtor(node, TREE_NODE_TYPE_CALL,
                         {.var = "ger"},
                         NULL,
                         ARG(node->left, ARG(node->right, NULL)));
            break;
        case OP_TYPE_EQ:
            TreeNodeCtor(node, TREE_NODE_TYPE_CALL,
                         {.var = "eq"},
                         NULL,
                         ARG(node->left, ARG(node->right, NULL)));
            break;
        case OP_TYPE_NEQ:
            TreeNodeCtor(node, TREE_NODE_TYPE_CALL,
                         {.var = "neq"},
                         NULL,
                         ARG(node->left, ARG(node->right, NULL)));
            break;
        case OP_TYPE_NOT:
            TreeNodeCtor(node, TREE_NODE_TYPE_CALL,
                         {.var = "not"},
                         NULL,
                         ARG(node->right, NULL));
            break;
        case OP_TYPE_OR:
            TreeNodeCtor(node, TREE_NODE_TYPE_CALL,
                         {.var = "or"},
                         NULL,
                         ARG(node->left, ARG(node->right, NULL)));
            break;
        case OP_TYPE_AND:
            TreeNodeCtor(node, TREE_NODE_TYPE_CALL,
                         {.var = "and"},
                         NULL,
                         ARG(node->left, ARG(node->right, NULL)));
            break;
    }
    
}

#undef CURR

