/*
** dot.c for ftrace in /home/leroy_v/Perso/ftrace
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Mon Mar 17 23:38:36 2014 vincent leroy
** Last update Tue Mar 18 00:00:07 2014 vincent leroy
*/

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include "ftrace.h"

static int fd = -1;

bool open_dot_file(char *filename)
{
    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
        return false;

    dprintf(fd, "digraph G {\n");

    return true;
}

void add_call_in_dot(t_proc *proc, char *function)
{
    char *name;
    char *file;

    if (size_of_stack() <= 0)
        return ;

    unsigned long addr = front_addr_to_stack();
    name = addr_to_name(proc, addr);
    file = addr_to_file(proc, addr);

    char buff[4096];
    if (name != NULL)
        snprintf(buff, 4096, "%s\n%s", name, file);
    else
        snprintf(buff, 4096, "NULL\n%s", file);

    dprintf(fd, "\t\"%s\" -> \"%s\"\n", buff, function);
}

void close_dot_file()
{
    if (fd == -1)
        return ;

    dprintf(fd, "}\n");
    close(fd);
}
