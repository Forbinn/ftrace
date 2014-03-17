/*
** read_proc_maps.c for ftrace in /home/leroy_v/Perso/ftrace
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Sat Mar 15 15:47:19 2014 vincent leroy
** Last update Mon Mar 17 23:06:31 2014 vincent leroy
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include "read_proc_maps.h"

static t_shared_elf* create_shared_elf(char *filename, char *begin, char *end, char *right, char *offset)
{
    t_shared_elf *shared_elf = calloc(1, sizeof(t_shared_elf));
    if (shared_elf == NULL)
        return NULL;

    if (filename != NULL)
    {
        shared_elf->filename = strdup(filename);
        shared_elf->elf = open_elf_file(filename);
    }
    shared_elf->begin_addr = strtoul(begin, NULL, 16);
    shared_elf->end_addr = strtoul(end, NULL, 16);
    shared_elf->offset = strtoul(offset, NULL, 16);
    strncpy(shared_elf->right, right, NB_RIGHT);
    return shared_elf;
}

static void delete_shared_elf(t_shared_elf *shared_elf)
{
    if (shared_elf == NULL)
        return ;

    if (shared_elf->filename != NULL)
    {
        free(shared_elf->filename);
        delete_elf(shared_elf->elf);
    }

    free(shared_elf);
}

bool read_executable(t_proc *proc, char *pathname)
{
    t_elf *elf;
    char name[BUFF_SIZE];

    if (pathname[0] != '/')
    {
        char cwd[BUFF_SIZE];
        if (getcwd(cwd, BUFF_SIZE) == NULL)
            return false;
        snprintf(name, BUFF_SIZE, "%s/%s", cwd, pathname);
    }
    else
        strncpy(name, pathname, BUFF_SIZE);

    if ((elf = open_elf_file(name)) == NULL)
        return false;

    if (proc->elf_list == NULL)
        proc->elf_list = list_create(NULL);

    proc->elf_list->data = elf;

    return true;
}

bool read_proc_maps(t_proc *proc)
{
    FILE *fd;
    char pathname[BUFF_SIZE];

    if (proc->elf_list == NULL || proc->elf_list->data == NULL)
    {
        errno = EINVAL;
        return false;
    }

    snprintf(pathname, BUFF_SIZE, "/proc/%d/maps", proc->pid);
    if ((fd = fopen(pathname, "r")) == NULL)
        return false;

    char line[BUFF_SIZE];
    while (fgets(line, BUFF_SIZE, fd) != NULL)
    {
        char *begin_addr = strtok(line, " -");
        char *end_addr = strtok(NULL, " ");
        char *right = strtok(NULL, " ");
        char *offset = strtok(NULL, " ");
        char *device = strtok(NULL, " ");
        char *inode = strtok(NULL, " ");
        char *filename = strtok(NULL, " \n");
        (void)device;
        (void)inode;

        if (filename != NULL)
        {
            t_elf *elf = list_user_data(proc->elf_list);
            if (strcmp(filename, elf->filename) == 0)
                continue;
        }

        t_shared_elf *shared_elf;
        shared_elf = create_shared_elf(filename, begin_addr, end_addr, right, offset);

        if (shared_elf == NULL)
        {
            fclose(fd);
            return false;
        }

        //eprintf("%lx => %lx (%lx : %lx): %s\n", shared_elf->begin_addr, shared_elf->end_addr, shared_elf->offset, shared_elf->begin_addr - shared_elf->offset, shared_elf->filename);
        list_push_back(proc->elf_list, shared_elf);
    }

    fclose(fd);
    return true;
}

void delete_proc(t_proc *proc)
{
    if (proc == NULL)
        return ;

    list_delete(proc->elf_list, (freelist)&delete_elf, (freedata)&delete_shared_elf);
}

char* addr_to_file(t_proc *proc, unsigned long addr)
{
    t_elm *itr;

    t_elf *elf = list_user_data(proc->elf_list);
    if (addr >= elf->file_begin && addr < elf->file_end)
        return elf->filename;

    for (itr = list_begin(proc->elf_list); itr != list_end(proc->elf_list); list_inc(&itr))
    {
        t_shared_elf *shared_elf = itr->data;
        if (addr >= shared_elf->begin_addr && addr < shared_elf->end_addr)
            return shared_elf->filename;
    }

    return NULL;
}
