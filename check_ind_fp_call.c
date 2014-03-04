/*
** check_ind_fp_call.c for ftrace in /home/leroy_v/Perso/ftrace
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Mon Mar 03 13:36:12 2014 vincent leroy
** Last update Tue Mar 04 20:51:48 2014 vincent leroy
*/

#include <sys/ptrace.h>

#include "ftrace.h"

/*
 * FF 14 24
 * FF 1*
 * 41 FF 14 24
 * 41 FF 1*
 */

bool check_ind_fp_call(t_prog *prog)
{
    int reg = (prog->tab[0] == 0xFF ? prog->tab[1] : prog->tab[2]) & 0x0F;
    unsigned long addr_in_reg = get_addr_in_register(reg, prog->tab[0] != 0xFF, &prog->regs);

    if (addr_in_reg == INVALID_ADDR)
        return false;

    unsigned long addr = ptrace(PTRACE_PEEKTEXT, prog->pid, addr_in_reg, NULL);
    if (addr == INVALID_ADDR)
        return false;

    push_addr_to_stack(addr);
    return true;
}
