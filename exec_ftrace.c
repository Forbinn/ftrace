/*
** exec_ftrace.c for ftrace in /home/leroy_v/Perso/ftrace
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Fri Feb 28 17:08:03 2014 vincent leroy
** Last update Mon Mar 17 22:15:32 2014 vincent leroy
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <sys/time.h>

#include "ftrace.h"
#include "read_proc_maps.h"

int run;

static unsigned long do_nothing(t_prog *prog)
{
    eprintf("call not treated for the moment %lx (rip = %llx)\n", prog->value, prog->regs.rip);
    (void)prog;
    return true;
}

static bool check_opcode(t_prog *prog, t_proc *proc)
{
    static const t_opcode op[] = {
        //{0x000000000000050FUL, 0x000000000000FFFFUL, &check_syscall},         // syscall
        //{0x000000000000340FUL, 0x000000000000FFFFUL, &check_syscall},         // sysenter
        {0x00000000000000E8UL, 0x00000000000000FFUL, &check_call},              // standard call
        {0x00000000002494FFUL, 0x0000000000FFFFFFUL, &check_off_fp_call},       // call like *0x3d8(%rsp)
        {0x00000000000094FFUL, 0x000000000000FFFFUL, &check_off_2_reg_call},    // call like *0x6c10(%rsp,%rax,1)
        {0x00000000000090FFUL, 0x000000000000F8FFUL, &check_off_fp_call},       // call like *0x578(%rax)
        {0x00000000002514FFUL, 0x0000000000FFFFFFUL, &do_nothing},              // call like *0x428
        {0x00000000002414FFUL, 0x0000000000FFFFFFUL, &check_ind_fp_call},       // call *(%rsp)
        {0x0000000000DD14FFUL, 0x0000000000FFFFFFUL, &check_off_2_reg_call},    // call like *0x0(,%rbx,8)
        {0x00000000000014FFUL, 0x000000000000FFFFUL, &check_2_reg_call},        // call like *(%rcx,%rax,1)
        {0x00000000000015FFUL, 0x000000000000FFFFUL, &check_off_fp_call},       // call like *0x51482c(%rip)
        {0x00000000002454FFUL, 0x0000000000FFFFFFUL, &check_off_fp_call},       // call like *0x70(%rsp)
        {0x00000000000054FFUL, 0x000000000000FFFFUL, &check_off_2_reg_call},    // call like *0x8(%rcx,%rax,1)
        {0x00000000000050FFUL, 0x000000000000F8FFUL, &check_off_fp_call},       // call like *0x58(%rax)
        {0x00000000000010FFUL, 0x000000000000F8FFUL, &check_ind_fp_call},       // call like *(%rax)
        {0x000000000000D0FFUL, 0x000000000000F8FFUL, &check_fp_call},           // call like *%rax
        {0x000000002494FF41UL, 0x00000000FFFFFFFFUL, &check_off_fp_call},       // call like *0x1d0(%r12)
        {0x000000000094FF41UL, 0x0000000000FFFFFFUL, &check_off_2_reg_call},    // call like *0x2c8(%r12,%rax,1)
        {0x000000000090FF41UL, 0x0000000000F8FFFFUL, &check_off_fp_call},       // call like *0xa0(%r8)
        {0x000000002454FF41UL, 0x00000000FFFFFFFFUL, &check_off_fp_call},       // call like *0x18(%r12)
        {0x000000000054FF41UL, 0x0000000000FFFFFFUL, &check_off_2_reg_call},    // call like *0x8(%r8,%rax,1)
        {0x000000000050FF41UL, 0x0000000000F8FFFFUL, &check_off_fp_call},       // call like *0x58(%r8)
        {0x000000002414FF41UL, 0x00000000FFFFFFFFUL, &check_ind_fp_call},       // call *(%r12)
        {0x000000000014FF41UL, 0x0000000000FFFFFFUL, &check_2_reg_call},        // call like *(%r10,%rax,1)
        {0x000000000010FF41UL, 0x0000000000F8FFFFUL, &check_ind_fp_call},       // call like *(%r8)
        {0x0000000000D0FF41UL, 0x0000000000F8FFFFUL, &check_fp_call},           // call like *%r8
        {0x000000000094FF42UL, 0x0000000000FFFFFFUL, &check_off_2_reg_call},    // call like *0xf58(%rbx,%r8,8)
        {0x000000000054FF42UL, 0x0000000000FFFFFFUL, &check_off_2_reg_call},    // call like *0x0(%rbp,%r8,1)
        {0x000000000014FF42UL, 0x0000000000FFFFFFUL, &check_2_reg_call},        // call like *(%rax,%r8,1)
        {0x000000000094FF43UL, 0x0000000000FFFFFFUL, &check_off_2_reg_call},    // call like *0x288(%r11,%r9,1)
        {0x000000000054FF43UL, 0x0000000000FFFFFFUL, &check_off_2_reg_call},    // call like *0x8(%r10,%r8,1)
        {0x000000000014FF43UL, 0x0000000000FFFFFFUL, &check_2_reg_call},        // call like *(%r11,%r8,1)
        {0x00000000000000C3UL, 0x00000000000000FFUL, &check_ret},               // ret
        {0x000000000000C3F3UL, 0x000000000000FFFFUL, &check_ret},               // repz ret
        {ULONG_MAX, ULONG_MAX, NULL}
    };
    int i;
    unsigned long addr;

    for (i = 0; op[i].function_to_call != NULL; ++i)
        if ((prog->value & op[i].mask) == op[i].opcode)
        {
            if ((addr = (*op[i].function_to_call)(prog)) == INVALID_ADDR)
                return false;
            else if (addr != 0) // 0 is return by ret and syscall
            {
                //eprintf("call\tfrom %#016llx to %#016lx", prog->regs.rip, addr);
                //eprintf(" (%s => %s)", addr_to_file(proc, prog->regs.rip), addr_to_file(proc, addr));
                //eprintf(" (%s)", addr_to_name(proc, addr));
                char *name = addr_to_name(proc, addr);
                char *file = addr_to_file(proc, addr);
                eprintf("%#016lx => %-50s (%s)", addr, name, file);
                eprintf("\n");

                push_addr_to_stack(addr);
            }
            else
            {
                //unsigned long tmp = ptrace(PTRACE_PEEKTEXT, prog->pid, prog->regs.rsp, NULL);
                //eprintf("ret\tfrom %#016llx to %#016lx\n", prog->regs.rip, tmp);
                if (size_of_stack() > 0)
                    pop_addr_to_stack();
            }
            return true;
        }

    return true;
}

static bool wait_for_start(t_proc *proc, t_prog *prog)
{
    bool ok = false;
    t_elf *elf = list_user_data(proc->elf_list);

    while (!ok && waitpid(prog->pid, NULL, WUNTRACED) != -1 && run)
    {
        if (ptrace(PTRACE_GETREGS, prog->pid, NULL, &prog->regs) == -1 && errno == ESRCH)
            break;

        if (prog->regs.rip >= elf->file_begin && prog->regs.rip < elf->file_end)
            ok = true;

        ptrace(PTRACE_SINGLESTEP, prog->pid, NULL, NULL);
    }

    return ok;
}

int CONST get_off(unsigned long value)
{
    t_value val;

    val.value = value;
    if (val.tab[0] != 0xFF)
        val.value >>= 8;

    if ((val.tab[1] != 0x94 && val.tab[1] != 0x54) || val.tab[1] == 0x15)
        return val.value >> 16;
    return val.value >> 24;
}

void split_2_reg_opcode(unsigned char op, unsigned char *s, int *reg1, int *reg2)
{
    *s = 1 << ((op & 0xC0) >> 6);
    *reg1 = (op & 0x07);
    *reg2 = (op & 0x38) >> 3;
}

unsigned long get_addr_in_register(int reg, bool reg_64, struct user_regs_struct *regs)
{
    switch (reg)
    {
        case 0x00: return reg_64 ? regs->r8 : regs->rax;
        case 0x01: return reg_64 ? regs->r9 : regs->rcx;
        case 0x02: return reg_64 ? regs->r10 : regs->rdx;
        case 0x03: return reg_64 ? regs->r11 : regs->rbx;
        case 0x04: return reg_64 ? regs->r12 : regs->rsp;
        case 0x05: return reg_64 ? regs->r13 : regs->rbp;
        case 0x06: return reg_64 ? regs->r14 : regs->rsi;
        case 0x07: return reg_64 ? regs->r15 : regs->rdi;
        default: return INVALID_ADDR;
    }
}

bool exec_ftrace(t_option *opt)
{
    int status;
    t_prog prog;
    t_proc proc;

    memset(&prog, 0, sizeof(t_prog));
    memset(&proc, 0, sizeof(t_proc));

    run = 1;
    prog.pid = opt->pid;
    proc.pid = opt->pid;

    catch_signal();

    if (!read_executable(&proc, opt->pathprogname))
    {
        eprintf("Unable to read executable file: %m\n");
        return false;
    }

    if (!wait_for_start(&proc, &prog))
        return false;

    if (!run)
    {
        delete_proc(&proc);
        return true;
    }

    if (!read_proc_maps(&proc))
    {
        eprintf("Unable to read proc file: %m\n");
        return false;
    }

    while (waitpid(prog.pid, &status, WUNTRACED) != -1 && run)
    {
        if (ptrace(PTRACE_GETREGS, prog.pid, NULL, &prog.regs) == -1 && errno == ESRCH)
            break;

        prog.value = ptrace(PTRACE_PEEKTEXT, prog.pid, prog.regs.rip, NULL);
        if (!check_opcode(&prog, &proc))
            eprintf("Canno't resolve call: %016lx at %016llx\n", prog.value, prog.regs.rip);

        ptrace(PTRACE_SINGLESTEP, prog.pid, NULL, NULL);
    }

    delete_proc(&proc);

    return true;
}
