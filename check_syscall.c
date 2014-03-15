/*
** check_syscall.c for ftrace in /home/leroy_v/Perso/ftrace
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Sat Mar 01 16:45:46 2014 vincent leroy
** Last update Wed Mar 12 18:03:34 2014 vincent leroy
*/

#include "ftrace.h"
#include "syscall.h"

unsigned long check_syscall(t_prog *prog)
{
    if (prog->regs.rax > NB_SYSCALL)
        return INVALID_ADDR;

    eprintf("syscall %016llx => %s\n", prog->regs.rip, tab_syscall[prog->regs.rax].name);
    return 0;
}
