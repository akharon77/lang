#ifndef COMPILER_H
#define COMPILER_H

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

#endif  // COMPILER_H
