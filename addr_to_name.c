/*
** addr_to_name.c for ftrace in /home/leroy_v/Perso/ftrace
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Fri Mar 07 18:19:31 2014 vincent leroy
** Last update Wed Mar 19 17:30:18 2014 vincent leroy
*/

#define _GNU_SOURCE
#include <string.h>
#include <sys/ptrace.h>

#include "ftrace.h"
#include "aelf.h"
#include "stdlist.h"
#include "syscall.h"

static t_list *function_list;

static void delete_function(t_function *function)
{
    if (function == NULL)
        return ;

    free(function->name);
    free(function);
}

static unsigned long get_addr_in_got(unsigned long addr, pid_t pid)
{
    static const unsigned int size_of_jump = 6;
    unsigned long value = ptrace(PTRACE_PEEKTEXT, pid, addr, NULL);
    unsigned int v = value >> 16;

    return v + addr + size_of_jump;
}

static unsigned long check_for_plt(unsigned long addr, unsigned long plt_begin, unsigned long plt_end, pid_t pid)
{
    if (addr < plt_begin || addr >= plt_end)
        return addr;

    return get_addr_in_got(addr, pid);
}

static char* check_in_process_it_self(unsigned long addr, t_elf *elf, t_proc *proc, bool *in_plt)
{
    if (addr < elf->file_begin || addr >= elf->file_end)
        return NULL;

    unsigned long save = addr;
    addr = check_for_plt(save, elf->plt_begin, elf->plt_end, proc->pid);
    if (in_plt != NULL && addr != save)
        *in_plt = true;

    return function_name_in_elf(elf, addr);
}

static char* check_in_shared_elf(unsigned long addr, t_shared_elf *shared_elf, t_proc *proc, bool *in_plt)
{
    t_elf *elf = shared_elf->elf;

    if (elf == NULL)
        return NULL;
    else if (addr < shared_elf->begin_addr || addr >= shared_elf->end_addr)
        return NULL;

    unsigned long plt_begin = elf->plt_begin + shared_elf->begin_addr - shared_elf->offset;
    unsigned long plt_end = elf->plt_end + shared_elf->begin_addr - shared_elf->offset;

    unsigned long save = addr;
    addr = check_for_plt(save, plt_begin, plt_end, proc->pid);
    if (in_plt != NULL && save != addr)
        *in_plt = true;

    return function_name_in_elf(elf, addr - shared_elf->begin_addr);
}

static int compare_function_addr(t_function *func, unsigned long *addr)
{
    return func->addr == *addr ? 0 : 1;
}

static char* search_name_with_addr(t_proc *proc, unsigned long addr, bool *in_plt)
{
    char *name;

    if ((name = check_in_process_it_self(addr, list_user_data(proc->elf_list), proc, in_plt)) != NULL)
        return name;

    t_elm *itr;
    for (itr = list_begin(proc->elf_list); itr != list_end(proc->elf_list); list_inc(&itr))
        if ((name = check_in_shared_elf(addr, itr->data, proc, in_plt)) != NULL)
            return name;

    return NULL;
}

static int compare_syscall(t_function *func, char *sysname)
{
    return strcmp(func->name, sysname);
}

t_function* syscall_to_name(int syscall)
{
    t_function *func;

    if ((func = list_search_data(function_list, tab_syscall[syscall].name, (cmp)&compare_syscall)) != NULL)
        return func;

    if ((func = calloc(1, sizeof(t_function))) == NULL)
        return NULL;

    func->is_syscall = true;
    asprintf(&func->name, "%s@syscall", tab_syscall[syscall].name);
    list_push_back(function_list, func);
    return func;
}

t_function* addr_to_name(t_proc *proc, unsigned long addr)
{
    t_function *func;

    if (function_list == NULL)
        function_list = list_create(NULL);

    func = list_search_data(function_list, &addr, (cmp)&compare_function_addr);
    if (func != NULL)
        return func;

    if ((func = calloc(1, sizeof(t_function))) == NULL)
        return NULL;

    func->is_syscall = false;
    func->addr = addr;
    if ((func->name = search_name_with_addr(proc, addr, &func->in_plt)) == NULL)
        asprintf(&func->name, "%#lx", addr);

    list_push_back(function_list, func);
    return func;
}

static bool is_in_plt(unsigned long addr, t_proc *proc)
{
    t_elf *elf = list_user_data(proc->elf_list);

    if (addr >= elf->plt_begin && addr < elf->plt_end)
        return true;
    else if (addr >= elf->file_begin && addr < elf->file_end)
        return false;

    t_elm *itr;
    for (itr = list_begin(proc->elf_list); itr != list_end(proc->elf_list); list_inc(&itr))
    {
        t_shared_elf *shared_elf = itr->data;
        if (shared_elf->elf == NULL)
            continue;

        unsigned long plt_begin = shared_elf->elf->plt_begin + shared_elf->begin_addr - shared_elf->offset;
        unsigned long plt_end = shared_elf->elf->plt_end + shared_elf->begin_addr - shared_elf->offset;

        if (addr >= plt_begin && addr < plt_end)
            return true;
        else if (addr >= shared_elf->begin_addr && addr < shared_elf->end_addr)
            return false;
    }

    return false;
}

static void sync_this_function(t_function *func, t_proc *proc)
{
    unsigned long addr = get_addr_in_got(func->addr, proc->pid);
    func->addr = ptrace(PTRACE_PEEKTEXT, proc->pid, addr, NULL);

    func->name = search_name_with_addr(proc, func->addr, NULL);
}

void sync_function_name_in_plt(t_proc *proc)
{
    if (function_list == NULL)
        return ;

    t_elm *itr;
    for (itr = list_begin(function_list); itr != list_end(function_list); list_inc(&itr))
    {
        t_function *func = itr->data;
        if (is_in_plt(func->addr, proc))
            sync_this_function(func, proc);
    }
}

void delete_list_function()
{
    list_delete(function_list, NULL, (freedata)&delete_function);
}
