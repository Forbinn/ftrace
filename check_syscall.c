/*
** check_syscall.c for ftrace in /home/leroy_v/Perso/ftrace
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Sat Mar 01 16:45:46 2014 vincent leroy
** Last update Sat Mar 01 21:35:12 2014 vincent leroy
*/

#include "ftrace.h"
#include "syscall.h"

bool check_syscall(t_prog *prog)
{
    if (prog->regs.rax > NB_SYSCALL)
        return false;

    eprintf("syscall %016llx => %s\n", prog->regs.rip, tab_syscall[prog->regs.rax].name);
    return true;
}
