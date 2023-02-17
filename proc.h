#ifndef PROC_H
#define PROC_H

#include <stdlib.h>

#include "stack.h"

enum CMD
{
    CMD_HLT,
    CMD_MUL,
    CMD_ADD,
    CMD_SUB,
    CMD_DIV,
    CMD_OUT,
    CMD_PUSH,
    CMD_POP,
    CMD_JMP,
    CMD_JB,
    CMD_JBE,
    CMD_JA, 
    CMD_JAE,
    CMD_JE,
    CMD_JNE,
    CMD_CALL,
    CMD_RET
};

int read_num (char *buf, int *ch);
void proc (Stack *st, char *buffer, int buf_len);
size_t read_file (char **buffer);
int check_st_size (Stack *st);
void scroll (char *buf, int *ch);
elem_t mem_reg_handle (int pos, elem_t value, int mode, int who);

#endif
