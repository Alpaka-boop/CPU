#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <sys/types.h>

typedef int elem_t;

typedef struct Stack {
    size_t size;
    elem_t *data;
    size_t capacity;
} Stack;

#define DELITED_NUM 30

const int START_SIZE_OF_STACK = 30;

void   stack_init   (Stack *st, size_t capacity);
void   stack_push   (Stack *st, elem_t value);
elem_t stack_pop    (Stack *st, elem_t *value);
void   stack_delete (Stack *st);
void   stack_resize (Stack *st);
void   add          (Stack *st);
void   sub          (Stack *st);
void   multi        (Stack *st);
void   div          (Stack *st);
void   out          (Stack *st);

#endif 
