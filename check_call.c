/*
** check_call.c for ftrace in /home/leroy_v/Perso/ftrace
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Sat Mar 01 18:06:41 2014 vincent leroy
** Last update Wed Mar 12 18:00:16 2014 vincent leroy
*/

#include "ftrace.h"

/*
 * E8 XX XX XX XX
 */
unsigned long check_call(t_prog *prog)
{
    static const unsigned int size_of_standard_call = sizeof(int) + 1;
    int value = (prog->value >> 8);
    unsigned long addr = prog->regs.rip + value + size_of_standard_call;

    return addr;
}
