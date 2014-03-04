/*
** check_2_reg_call.c for ftrace in /home/leroy_v/Perso/ftrace
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Mon Mar 03 18:03:48 2014 vincent leroy
** Last update Tue Mar 04 20:50:58 2014 vincent leroy
*/

#include <sys/ptrace.h>

#include "ftrace.h"

/*
 * FF 14 **
 * 41 FF 14 **
 * 42 FF 14 **
 * 43 FF 14 **
 */

bool check_2_reg_call(t_prog *prog)
{
    unsigned char s;
    int reg1;
    int reg2;

    split_2_reg_opcode(prog->tab[0] == 0xFF ? prog->tab[2] : prog->tab[3], &s, &reg1, &reg2);

    bool reg1_64 = prog->tab[0] == 0x41 || prog->tab[0] == 0x43;
    bool reg2_64 = prog->tab[0] == 0x42 || prog->tab[0] == 0x43;
    unsigned long addr_in_reg1 = get_addr_in_register(reg1, reg1_64, &prog->regs);
    unsigned long addr_in_reg2 = get_addr_in_register(reg2, reg2_64, &prog->regs);

    if (addr_in_reg1 == INVALID_ADDR || addr_in_reg2 == INVALID_ADDR)
        return false;

    unsigned long addr = ptrace(PTRACE_PEEKTEXT, prog->pid, addr_in_reg1 + (s * addr_in_reg2), NULL);
    if (addr == INVALID_ADDR)
        return false;

    push_addr_to_stack(addr);
    return true;
}
