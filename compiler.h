#ifndef COMPILER_H
#define COMPILER_H

#include "tree.h"
#include "stdint.h"
#include "stack.h"

const int32_t INF = 1e9;

struct LocalVar
{
    char    *name;
    int32_t  ptr;
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
};

struct CompilerInfo
{
    Stack     namesp;
    Stack     fun_table;
    Stack     rbp;

    int32_t   if_cnt;
    int32_t   loop_cnt;
};

#define TYPE(name) void COMPILE_##name(TreeNode *node, CompilerInfo *info, int32_t fd);

#include "tree_node_types.h"
#undef TYPE

void    Compile         (TreeNode *node, CompilerInfo *info, int32_t fd);
int32_t GetVarPointer   (Stack *stk, const char *name);
int32_t AddLocalVar     (Stack *stk, const char *name);
void    RepeatNameTable (Stack *stk);
void    MakeNamespace   (Stack *stk);
void    CloseNamespace  (Stack *stk);
void    PreCompileOp    (TreeNode *node);

#endif  // COMPILER_H

