#ifndef COMPILER_H
#define COMPILER_H

#include "tree.h"
#include "stdint.h"
#include "stack.h"

const int32_t INF = 1e9;

struct VarPtr
{
    int32_t ptr;
    int32_t depth;
};

struct LocalVar
{
    char  *name;
    Stack  mem;
};

struct FunctionInfo
{
    char    *name;
    int32_t  arg_cnt;
};

struct NameTable
{
    Stack   stk;
    int32_t free_ptr;
    int32_t depth;
};

struct CompilerInfo
{
    NameTable name_table;
    Stack     fun_table;

    int32_t   if_cnt;
    int32_t   loop_cnt;
};

#define TYPE(name) void COMPILE_##name(TreeNode *node, CompilerInfo *info, int32_t fd);

#include "tree_node_types.h"
#undef TYPE

void    Compile        (TreeNode *node, CompilerInfo *info, int32_t fd);
int32_t GetVarPointer  (NameTable *name_table, const char *name);
int32_t AddLocalVar    (NameTable *name_table, const char *name);
void    MakeNamespace  (NameTable *name_table);
void    CloseNamespace (NameTable *name_table);

#endif  // COMPILER_H

