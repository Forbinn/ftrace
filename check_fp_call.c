/*
** check_fp_call.c for ftrace in /home/leroy_v/Perso/ftrace
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Mon Mar 03 13:14:00 2014 vincent leroy
** Last update Wed Mar 12 18:01:17 2014 vincent leroy
*/

#include "ftrace.h"

/*
 * FF D*
 * 41 FF D*
 */
unsigned long check_fp_call(t_prog *prog)
{
    int reg = (prog->tab[0] == 0xFF ? prog->tab[1] : prog->tab[2]) & 0x0F;
    if (reg == 0x04 && prog->tab[0] == 0xFF) // %rsp canno't be use at function pointer
        return INVALID_ADDR;

    unsigned long addr = get_addr_in_register(reg, prog->tab[0] != 0xFF, &prog->regs);
    return addr;
}
