/*
** check_syscall.c for ftrace in /home/leroy_v/Perso/ftrace
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Sat Mar 01 16:45:46 2014 vincent leroy
** Last update Tue Mar 18 00:09:45 2014 vincent leroy
*/

#include "ftrace.h"
#include "syscall.h"

unsigned long check_syscall(t_prog *prog)
{
    if (prog->regs.rax > NB_SYSCALL)
        return INVALID_ADDR;

    return 0;
}
