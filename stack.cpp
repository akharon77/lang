#include <time.h>
#include <stdlib.h>
#include "stack.h"

void StackCtor (Stack *stk, int64_t capacity, int32_t elem_size) 
{
    ASSERT(!isBadPtr(stk));

    void *data = calloc(capacity, elem_size);

    ASSERT(!isBadPtr(stk));
    stk->data       = data;
    stk->size       = 0;
    stk->capacity   = capacity;
    stk->elem_size  = elem_size;
}

void StackDtor(Stack *stk)
{
    ASSERT(!isBadPtr(stk));

    free(stk->data);
}

void StackPush(Stack *stk, void *val)
{
    ASSERT(!isBadPtr(stk));
    ASSERT(!isBadPtr(val));

    StackResize (stk, stk->size + 1);
    memcpy((void*) ((char*) stk->data + stk->elem_size * (stk->size - 1)), val, stk->elem_size);
}

void StackGet(Stack *stk, int32_t ind, void *val)
{
    ASSERT(!isBadPtr(stk));
    ASSERT(!isBadPtr(val));
    ASSERT(StackGetSize(stk) > 0);
    ASSERT(-1 < ind && ind < stk->size);
    
    memcpy(val, (void*) ((char*) stk->data + stk->elem_size * ind), stk->elem_size);
}

void StackTop(Stack *stk, void *val)
{
    ASSERT(!isBadPtr(stk));
    ASSERT(!isBadPtr(val));
    ASSERT(StackGetSize(stk) > 0);
    
    StackGet(stk, stk->size - 1, val);
}

void StackPop(Stack *stk, void *val)
{
    ASSERT(!isBadPtr(stk));
    ASSERT(!isBadPtr(val));
    ASSERT(StackGetSize(stk) > 0);
    
    StackTop(stk, val);
    
    int64_t prevSize = stk->size;
    StackResize(stk, stk->size - 1);
}

void StackResize(Stack *stk, int64_t size)
{
    ASSERT(!isBadPtr(stk));

    if (size > stk->capacity)
    {
        int64_t inc_coeff_capacity = stk->capacity * StackGetCoeff(stk);
        StackRealloc(stk, max(size, inc_coeff_capacity));
    }
    else if (size < stk->size)
    {
        int64_t coeff = StackGetCoeff(stk);
        int64_t dec_coeff_capacity = stk->capacity / (coeff * coeff);

        if (size < dec_coeff_capacity)
            StackRealloc(stk, dec_coeff_capacity);
    }

    stk->size = size;
}

void StackRealloc(Stack *stk, int64_t capacity)
{
    ASSERT(!isBadPtr(stk));

    if (capacity == stk->capacity)
        return;

    void *new_data = realloc(stk->data, stk->elem_size * capacity);
    ASSERT(!isBadPtr(new_data));

    stk->data       = new_data;
    stk->capacity   = capacity;
}

int64_t StackGetSize(Stack *stk)
{
    ASSERT(!isBadPtr(stk));
    return stk->size;
}

int64_t StackGetCapacity(Stack *stk)
{
    ASSERT(!isBadPtr(stk));
    return stk->capacity;
}

int64_t StackGetCoeff(Stack *stk)
{
    ASSERT(!isBadPtr(stk));
    return 2; // TODO: formula
}

bool isBadPtr(void *ptr)
{
    if (ptr == NULL)
        return true;

    int nullfd = open("/dev/random", O_WRONLY);
    bool res = write(nullfd, (char*) ptr, 1) < 0;

    close(nullfd);
    return res;
}

int64_t max(int64_t a, int64_t b)
{
    return a > b ? a : b;
}
