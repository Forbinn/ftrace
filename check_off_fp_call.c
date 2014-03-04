/*
** check_off_fp_call.c for ftrace in /home/leroy_v/Perso/ftrace
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Mon Mar 03 15:26:37 2014 vincent leroy
** Last update Tue Mar 04 20:58:00 2014 vincent leroy
*/

#include <sys/ptrace.h>

#include "ftrace.h"

/*
 * FF 94 24 XX XX XX XX
 * FF 9* XX XX XX XX
 * FF 54 24 XX
 * FF 5* XX
 * FF 15 XX XX XX XX
 * 41 FF 94 24 XX XX XX XX
 * 41 FF 9* XX XX XX XX
 * 41 FF 54 24 XX
 * 41 FF 5* XX
 */

bool check_off_fp_call(t_prog *prog)
{
    static const unsigned int size_of_off_call = sizeof(int) + 1;
    int off = get_off(prog->value);
    int reg = (prog->tab[0] == 0xFF ? prog->tab[1] : prog->tab[2]) & 0x0F;
    unsigned long addr_in_reg = get_addr_in_register(reg, prog->tab[0] != 0xFF, &prog->regs);
    unsigned long addr;

    if (addr_in_reg == INVALID_ADDR)
        return false;

    if ((prog->tab[1] & 0xF0) == 0x50 || (prog->tab[0] == 0x41 && (prog->tab[2] & 0xF0) == 0x50))
        off &= 0xFF;

    if (prog->tab[1] == 0x15)
        addr = prog->regs.rip + (unsigned int)off + size_of_off_call;
    else
        addr = ptrace(PTRACE_PEEKTEXT, prog->pid, addr_in_reg + off, NULL);

    if (addr == INVALID_ADDR)
        return false;

    push_addr_to_stack(addr);
    return true;
}
