#include <stdlib.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdio.h>

#include "proc.h"
#include "stack.h"

#define filename "factor.bin"
#define mem_cap 100
#define rg_cap 6

elem_t rg[rg_cap]   = {};
elem_t mem[mem_cap] = {};

char MODE_MASK = 0b00000100;
char NUMS_MASK = 0b00000010;
char REGS_MASK = 0b00000001;

enum MODE
{
    OUT,
    IN
};

enum WHO
{
    MEM,
    REG
};

elem_t mem_reg_handle (int pos, elem_t value, int mode, int who)
{
    int ret = 0;

    if (pos < mem_cap * (who == MEM) + rg_cap * (who == REG))
    {
        if (mode == IN && who == MEM)
        {
            mem[pos] = value;
        }
        else if (mode == OUT && who == MEM)
        {
            ret = mem[pos];
        }
        else if (mode == IN && who == REG)
        {
            rg[pos] = value;
        }
        else
        {
            ret = rg[pos];
        }
    }
    else 
    {
        printf("segmentation fault\n");
        abort();
    }

    return ret;
}

int read_num (char *buf, int *ch)
{
    int  res   = 0;
    bool minus = false;

    if (buf[*ch] == '-')
    {
        (*ch)++;
        minus = true;
    }

    while (buf[*ch] != '\n')
    {
        res = res * 10 + (int) (buf[*ch] - '0');
        (*ch)++;
    }

    if (minus)
    {
        res *= -1;
    }

    return res;
}

void proc (Stack *st, char *buffer, int buf_len)
{
    elem_t reg = 0;

    for (int ch = 0; ch < buf_len; ch++)
    {
        switch (buffer[ch++] >> 3)
        {
            case CMD_HLT:
                return;

            case CMD_MUL:
                multi(st);
                break;

            case CMD_ADD:
                add(st);
                break;

            case CMD_SUB:
                sub(st);
                break;

            case CMD_DIV:
                div(st);
                break;

            case CMD_OUT:
                out(st);
                break;
            
            case CMD_PUSH:
                if (buffer[ch - 1] & MODE_MASK)
                {
                    if (buffer[ch - 1] & NUMS_MASK && buffer[ch - 1] & REGS_MASK)
                    {
                        reg = mem_reg_handle(buffer[ch] - 'a', 0, OUT, REG);
                        ch += 1;
                        stack_push(st, mem_reg_handle(reg + read_num(buffer, &ch), 0, OUT, MEM));
                    }
                    else if (buffer[ch - 1] & REGS_MASK)
                    {
                        reg = mem_reg_handle(buffer[ch] - 'a', 0, OUT, REG);
                        stack_push(st, mem_reg_handle(reg, 0, OUT, MEM));
                    }
                    else 
                    {
                        stack_push(st, mem_reg_handle(read_num(buffer, &ch), 0, OUT, MEM));
                    }

                }

                else
                {
                    if (buffer[ch - 1] & REGS_MASK)
                    {
                        stack_push(st, reg);
                    }
                    else
                    {
                        int num = read_num(buffer, &ch);
                        stack_push(st, num);
                    }
                }

                break;

            case CMD_POP:
                if (buffer[ch - 1] & MODE_MASK)
                {
                    if (buffer[ch - 1] & NUMS_MASK && buffer[ch - 1] & REGS_MASK)
                    {
                        reg = mem_reg_handle(buffer[ch] - 'a', 0, OUT, REG);
                        ch += 1;
                        mem_reg_handle(reg + read_num(buffer, &ch), stack_pop(st, nullptr), IN, MEM);
                    }
                    else if (buffer[ch - 1] & REGS_MASK)
                    {
                        reg = mem_reg_handle(buffer[ch] - 'a', 0, OUT, REG);
                        mem_reg_handle(reg, stack_pop(st, nullptr), IN, MEM);
                    }
                    else 
                    {
                        mem_reg_handle(read_num(buffer, &ch), stack_pop(st, nullptr), IN, MEM);
                    }
                }

                else
                {
                    if (buffer[ch - 1] & REGS_MASK)
                    {
                        mem_reg_handle(buffer[ch] - 'a', stack_pop(st, nullptr), IN, REG);
                    }
                    else
                    {
                        stack_pop(st, nullptr);
                    }
                }

                break;
            
            case CMD_JMP:
                ch = read_num(buffer, &ch);
                break;

            case CMD_JB:
                if (check_st_size(st) && stack_pop(st, nullptr) < stack_pop(st, nullptr))
                    ch = read_num(buffer, &ch); 
                else
                    scroll(buffer, &ch);

                break;

            case CMD_JBE:
                if (check_st_size(st) && stack_pop(st, nullptr) <= stack_pop(st, nullptr))
                    ch = read_num(buffer, &ch); 
                else
                    scroll(buffer, &ch);

                break;

            case CMD_JA:
                if (check_st_size(st) && stack_pop(st, nullptr) > stack_pop(st, nullptr))
                    ch = read_num(buffer, &ch); 
                else
                    scroll(buffer, &ch);

                break;

            case CMD_JAE:
                if (check_st_size(st) && stack_pop(st, nullptr) <= stack_pop(st, nullptr))
                    ch = read_num(buffer, &ch); 
                else
                    scroll(buffer, &ch);

                break;
            
            case CMD_JE:
                if (check_st_size(st) && stack_pop(st, nullptr) == stack_pop(st, nullptr))
                    ch = read_num(buffer, &ch); 
                else
                    scroll(buffer, &ch);

                break;

            case CMD_JNE:
                if (check_st_size(st) && stack_pop(st, nullptr) != stack_pop(st, nullptr))
                    ch = read_num(buffer, &ch); 
                else
                    scroll(buffer, &ch);

                break;

            case CMD_CALL:
                rg[5] = ch + 2;
                ch = read_num(buffer, &ch);
                break;

            case CMD_RET:
                ch = rg[5];

            default:
                break;
        }
    }
}

void scroll (char *buf, int *ch)
{
    while (buf[*ch] != '\n')
    {
        (*ch)++;
    }
}

int check_st_size (Stack *st)
{
    if (st->size < 2)
    {
        printf("few elements in stack to use comparative jump command\n");
        return 0;
    }

    return 1;
}

size_t read_file (char **buffer)
{
    FILE *input_file = fopen(filename, "rb");
    assert(input_file);

    struct stat st = {};
    stat(filename, &st);
    size_t number_of_characters = st.st_size / sizeof(char);
    
    *buffer = (char *) calloc(number_of_characters + 1, sizeof(char));
    fread((*buffer), sizeof(char), number_of_characters, input_file);
    fclose(input_file);

    return number_of_characters;
}
