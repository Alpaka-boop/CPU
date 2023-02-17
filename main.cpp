#include <stdlib.h>

#include "stack.h"
#include "proc.h"

int main ()
{
    Stack st = {};
    char *buffer = nullptr;

    size_t num_of_char = read_file(&buffer);

    stack_init(&st, START_SIZE_OF_STACK);
    proc(&st, buffer, num_of_char);
    stack_delete(&st);

    return 0;
}
