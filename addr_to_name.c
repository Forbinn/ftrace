/*
** addr_to_name.c for ftrace in /home/leroy_v/Perso/ftrace
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Fri Mar 07 18:19:31 2014 vincent leroy
** Last update Mon Mar 17 22:33:10 2014 vincent leroy
*/

#include <sys/ptrace.h>

#include "ftrace.h"
#include "aelf.h"

static unsigned long check_for_plt(unsigned long addr, unsigned long plt_begin, unsigned long plt_end, pid_t pid)
{
    static const unsigned int size_of_jump = 6;
    unsigned long value;
    unsigned int v;

    if (addr < plt_begin || addr >= plt_end)
        return addr;

    value = ptrace(PTRACE_PEEKTEXT, pid, addr, NULL);
    v = value >> 16;
    return v + addr + size_of_jump;
}

char* addr_to_name(t_proc *proc, unsigned long addr)
{
    char *name;
    t_elf *elf = list_user_data(proc->elf_list);
    t_elm *itr;

    if (addr >= elf->file_begin && addr < elf->file_end)
    {
        addr = check_for_plt(addr, elf->plt_begin, elf->plt_end, proc->pid);

        if ((name = function_name_in_elf(elf, addr)) != NULL)
            return name;
    }

    for (itr = list_begin(proc->elf_list); itr != list_end(proc->elf_list); list_inc(&itr))
    {
        t_shared_elf *shared_elf = itr->data;
        if (shared_elf->elf == NULL)
            continue;

        if (addr < shared_elf->begin_addr || addr >= shared_elf->end_addr)
            continue;

        unsigned long plt_begin = shared_elf->elf->plt_begin + shared_elf->begin_addr;
        unsigned long plt_end = shared_elf->elf->plt_end + shared_elf->begin_addr;
        addr = check_for_plt(addr, plt_begin, plt_end, proc->pid);

        if ((name = function_name_in_elf(shared_elf->elf, addr - shared_elf->begin_addr)) != NULL)
            return name;
    }

    return NULL;
}
