#ifndef COMPILER_H
#define COMPILER_H

#include "tree.h"
#include "stdint.h"
#include "stack.h"

extern const char    *STD_LANG_PATH;
extern const char    *STD_ASM_PATH;
extern const int32_t  GLOB_SEC_PTR;
extern const int32_t  INF;
extern const int32_t  NO_VAR;

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
    Stack     globsp;
    Stack     fun_table;
    Stack     rbp;

    int32_t   if_cnt;
    int32_t   loop_cnt;
};

#define TYPE(name) void COMPILE_##name(TreeNode *node, CompilerInfo *info, int32_t fd, int32_t depht); \
                   void DECOMPILE_##name(TreeNode *node, int32_t fd, int32_t depth);

#include "tree_node_types.h"
#undef TYPE

void    CompilerInfoCtor  (CompilerInfo *info);
void    CompilerInfoDtor  (CompilerInfo *info);
void    CompileProgram    (TreeNode *node, CompilerInfo *info, int32_t fd);
void    Compile           (TreeNode *node, CompilerInfo *info, int32_t fd, int32_t depth);
void    Decompile         (TreeNode *node, int32_t fd, int32_t depth);
void    PrintVarPointer   (const char *name, CompilerInfo *info, int32_t fd);
int32_t GetGlobVarPointer (Stack *stk, const char *name);
int32_t GetVarPointer     (Stack *stk, const char *name);
int32_t AddGlobalVar      (Stack *stk, const char *name);
int32_t AddLocalVar       (Stack *stk, const char *name);
void    RepeatNameTable   (Stack *stk);
void    MakeNamespace     (Stack *stk);
void    CloseNamespace    (Stack *stk);
void    PreCompileOp      (TreeNode *node);
void    AddStdAsmLib      (int32_t fd);
void Tabstabe(int32_t fd, int32_t depth);

#endif  // COMPILER_H

