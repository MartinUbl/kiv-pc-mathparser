#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

/**
 * Creates stack structure with space of specified size for its elements
 */
c_stack* stck_create(int size)
{
    c_stack *stck = (c_stack*)malloc(sizeof(c_stack));
    if (stck == NULL)
        return NULL;

    stck->size = size;
    stck->curr = STCK_INVALID;
    stck->elements = (void**)malloc(sizeof(void*)*size);
    memset(stck->elements, 0, sizeof(void*)*size);

    return stck;
}

/**
 * Destroys stack structure and destroys even all unpopped elements!
 */
void stck_destroy(c_stack *stck)
{
    /* not popped elements will be deleted */
    for (; stck->curr >= 0; stck->curr--)
    {
        if (stck->elements[stck->curr] != NULL)
            free(stck->elements[stck->curr]);
    }

    free(stck->elements);
    free(stck);
}

/**
 * Pops value from stack and returns it, or returns NULL if the stack is empty
 */
void* stck_pop(c_stack *stck)
{
    /* STCK_INVALID is -1, so it's set automatically when the stack is emptied */
    if (stck->curr == STCK_INVALID)
        return NULL;

    return stck->elements[stck->curr--];
}

/**
 * Peeks current element from the top of the stack, but remains
 * stack unchanged
 */
void* stck_peek(c_stack *stck)
{
    return stck_get(stck, stck->curr);
}

/**
 * Gets element from stack from specified position
 */
void* stck_get(c_stack *stck, int pos)
{
    /* STCK_INVALID is -1, so it's set automatically when the stack is emptied */
    if (stck->curr == STCK_INVALID)
        return NULL;

    return stck->elements[pos];
}

/**
 * Pushes value on stack
 */
void stck_push(c_stack *stck, void *el)
{
    if (stck->curr == stck->size - 1)
        return;

    stck->elements[++stck->curr] = el;
}
