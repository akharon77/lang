#ifndef STACK_H
#define STACK_H

#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "stack_struct.h"
#include "stack_debug.h"

void        StackCtor             (Stack *stk, int64_t capacity, int32_t elem_size);

void        StackDtor             (Stack *stk);

void        StackPush             (Stack *stk, void *val);

void        StackGet              (Stack *stk, int32_t ind, void *val);
void        StackTop              (Stack *stk, void *val);
void        StackPop              (Stack *stk, void *val);

void        StackResize           (Stack *stk, int64_t size);
void        StackRealloc          (Stack *stk, int64_t capacity);

int64_t     StackGetSize          (Stack *stk);
int64_t     StackGetCapacity      (Stack *stk);
int64_t     StackGetCoeff         (Stack *stk);

bool        isBadPtr              (void *ptr);

int64_t     max                   (int64_t a, int64_t b);

#endif  // STACK_H
