#ifndef STACK_STRUCT_H
#define STACK_STRUCT_H

struct Stack
{
    int64_t  size;
    int64_t  capacity;

    int32_t  elem_size;
    void    *data;
};

#endif  // STACK_STRUCT_H
