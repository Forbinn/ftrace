/*
** ftrace.h for ftrace in /home/leroy_v/Perso/ftrace
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Fri Feb 28 15:16:59 2014 vincent leroy
** Last update Thu Mar 13 21:05:04 2014 vincent leroy
*/

#ifndef FTRACE_H_
# define FTRACE_H_

#include <sys/types.h>
#include <sys/user.h>
#include <stdbool.h>

#define ATTR(x, ...)     __attribute__((x, ##__VA_ARGS__))
#define NORETURN        ATTR(noreturn)
#define UNUSED          ATTR(unused)
#define CONST           ATTR(const)

#ifndef eprintf
#include <stdio.h>

#define eprintf(...)    fprintf(stdout, __VA_ARGS__)
#endif

#define INVALID_ADDR    ((unsigned long)-1)

extern int run; // Define in exec_ftrace.c

typedef struct s_option
{
    char    *progname;
    char    *pathprogname;
    char    **argument;
    pid_t   pid;
} t_option;

typedef union u_value
{
    unsigned long value;
    unsigned char tab[sizeof(unsigned long)];
} t_value;

typedef struct s_prog
{
    pid_t                   pid;
    struct user_regs_struct regs;
    union
    {
        unsigned long       value;
        unsigned char       tab[sizeof(unsigned long)];
    };
} t_prog;

typedef unsigned long (*op_callback)(t_prog*);

typedef struct so_opcode
{
    unsigned long   opcode;
    unsigned long   mask;
    op_callback     function_to_call;
} t_opcode;

bool exec_program(t_option *opt);
bool exec_ftrace(t_option *opt);
int CONST get_off(unsigned long value);
void split_2_reg_opcode(unsigned char op, unsigned char *s, int *reg1, int *reg2);
unsigned long get_addr_in_register(int reg, bool reg_64, struct user_regs_struct *regs);
void catch_signal();

unsigned long check_syscall(t_prog *prog);

unsigned long check_ret(t_prog *prog);

unsigned long check_call(t_prog *prog);
unsigned long check_fp_call(t_prog *prog);
unsigned long check_ind_fp_call(t_prog *prog);
unsigned long check_off_fp_call(t_prog *prog);
unsigned long check_2_reg_call(t_prog *prog);
unsigned long check_off_2_reg_call(t_prog *prog);

void push_addr_to_stack(unsigned long addr);
void pop_addr_to_stack();
unsigned long front_addr_to_stack();
void delete_stack();
int size_of_stack();

char* addr_to_name(unsigned long addr);
void delete_elf_list();

#endif /* !FTRACE_H_ */
