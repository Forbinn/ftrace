/*
** check_ret.c for ftrace in /home/leroy_v/Perso/ftrace
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Sat Mar 01 21:28:28 2014 vincent leroy
** Last update Mon Mar 17 23:23:28 2014 vincent leroy
*/

#include <sys/ptrace.h>

#include "ftrace.h"

unsigned long check_ret(t_prog *prog)
{
    unsigned long addr;

    addr = ptrace(PTRACE_PEEKTEXT, prog->pid, prog->regs.rsp, NULL);
    if (addr == INVALID_ADDR)
        return INVALID_ADDR;

    return addr;
}
