/*
** addr_to_name.c for ftrace in /home/leroy_v/Perso/ftrace
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Fri Mar 07 18:19:31 2014 vincent leroy
** Last update Mon Mar 17 21:00:51 2014 vincent leroy
*/

#include "ftrace.h"
#include "aelf.h"

char* addr_to_name(t_proc *proc, unsigned long addr)
{
    char *name;
    t_elf *elf = list_user_data(proc->elf_list);
    t_elm *itr;

    if (addr >= elf->file_begin && addr < elf->file_end)
        if ((name = function_name_in_elf(elf, addr)) != NULL)
            return name;

    for (itr = list_begin(proc->elf_list); itr != list_end(proc->elf_list); list_inc(&itr))
    {
        t_shared_elf *shared_elf = itr->data;
        if (shared_elf->elf == NULL)
            continue;

        if (addr < shared_elf->begin_addr || addr >= shared_elf->end_addr)
            continue;

        if ((name = function_name_in_elf(shared_elf->elf, addr - shared_elf->begin_addr)) != NULL)
            return name;
    }

    return NULL;
}
