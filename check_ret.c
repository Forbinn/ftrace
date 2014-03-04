/*
** check_ret.c for ftrace in /home/leroy_v/Perso/ftrace
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Sat Mar 01 21:28:28 2014 vincent leroy
** Last update Tue Mar 04 13:38:44 2014 vincent leroy
*/

#include "ftrace.h"

bool check_ret(t_prog *prog)
{
    (void)prog;

    if (size_of_stack() == 0)
        return true;

    pop_addr_to_stack();
    return true;
}
