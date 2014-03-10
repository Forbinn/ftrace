/*
** main.c for elf in /home/leroy_v/Perso/ftrace/elf
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Tue Mar 04 21:40:34 2014 vincent leroy
** Last update Mon Mar 10 17:00:23 2014 vincent leroy
*/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "aelf.h"

int fd = -1;

void aff_list(t_elf *elf);

void aff_dep_file(t_dep_file *dep_file)
{
    dprintf(fd, "\"%s\"\n", dep_file->pathname);
    eprintf("%s (%s)\n", dep_file->filename, dep_file->pathname);
    if (!dep_file->elf)
        return ;

    aff_list(dep_file->elf);
}

void aff_list(t_elf *elf)
{
    if (elf == NULL)
        return ;
    if (elf->dep_list == NULL)
        return ;

    t_elm *elm = elf->dep_list->first;
    if (elm == NULL)
        return ;

    eprintf("dump of %s begin\n", elf->filename);
    do
    {
        t_dep_file *dep_file = elm->data;
        eprintf("\t%20s => ", elf->filename);
        dprintf(fd, "\"%s\" -> ", elf->filename);
        aff_dep_file(dep_file);
        elm = elm->next;
    }
    while (elm != elf->dep_list->first);
    eprintf("dump of %s finished\n", elf->filename);
}

int main(int ac, char **av)
{
    t_elf *elf;

    if (ac != 2)
    {
        fprintf(stderr, "Usage: %s file.so\n", av[0]);
        return 1;
    }

    if ((elf = open_elf_file(av[1], false)) == NULL)
    {
        fprintf(stderr, "Unable to open elf file\n");
        return 1;
    }

    if ((fd = open("file.dot", O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
    {
        fprintf(stderr, "Unable to open file file.dot: %m\n");
        return 1;
    }

    dprintf(fd, "digraph G\n{\n");

    aff_list(elf);

    dprintf(fd, "\n}\n");

    function_name_in_elf(elf, 0);

    delete_elf(elf);
    delete_dependency_list();
    close(fd);

    return 0;
}
