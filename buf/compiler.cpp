#include <stdio.h>

#include "compiler.h"
#include "dsl.h"

#define CURR node

void Compile(TreeNode *node, Stack *name_table, int32_t depth, int32_t fd)
{
    #define TYPE(name)                                   \
        case TREE_NODE_TYPE_##name:                      \
            Compile_##name(CURR, name_table, depth, fd); \
            break;

    switch (GET_TYPE(CURR))
    {
        #include "tree_node_types.h"
    }
    #undef TYPE
}

void Compile_NUM(TreeNode *node, Stack *name_table, int32_t depth, int32_t fd)
{
    dprintf(fd, "push %ld\n", GET_NUM(CURR));
}

void Compile_VAR(TreeNode *node, Stack *name_table, int32_t depth, int32_t fd)
{
    dprintf(fd, "push [%ld]\n", GetVarPointer(name_table, GET_VAR(CURR)));
}

int32_t GetVarPointer(Stack *name_table, const char *name)
{
    for (int32_t i = 0; i < name_table->size; ++i)
    {
        LocalVar *var = (LocalVar*) StackGet(name_table, i);

        if (strcasecmp(var->name, name) == 0)
            return ((VarPtr*) StackGet(&var->mem, var->mem.size - 1))->ptr;
    }

    return 0;
}

void Compile_OP(TreeNode *node, Stack *name_table, int32_t depth, int32_t fd)
{
    Compile(LEFT,  name_table, depth, fd);
    Compile(RIGHT, name_table, depth, fd);

    #define TYPE(op, cmd)            \
        case OP_TYPE_##op:           \
            dprintf(fd, "%s\n", cmd) \
            break;

    switch (GET_OP(CURR))
    {
        #include "op_types.h"
    }
    #undef TYPE
}

#undef CURR

