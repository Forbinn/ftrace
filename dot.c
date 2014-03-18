/*
** dot.c for ftrace in /home/leroy_v/Perso/ftrace
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Mon Mar 17 23:38:36 2014 vincent leroy
** Last update Tue Mar 18 02:46:10 2014 vincent leroy
*/

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ftrace.h"
#include "syscall.h"
#include "stdlist.h"

typedef struct s_dot_name
{
    char *first_name;
    char *second_name;
    int count;
} t_dot_name;

static t_list *name_list;
static int fd = -1;

static void addr_to_fullname(t_proc *proc, unsigned long addr, char *buff, int size)
{
    char *name = addr_to_name(proc, addr);
    char *file = addr_to_file(proc, addr);

    if (file == NULL && name == NULL)
        snprintf(buff, size, "%#016lx", addr);
    else if (name == NULL)
        snprintf(buff, size, "%#016lx\n%s", addr, file);
    else
        snprintf(buff, size, "%s\n%s", name, file);
}

static t_dot_name* create_dot_name(char *name1, char *name2)
{
    t_dot_name *dot_name;

    if ((dot_name = calloc(1, sizeof(t_dot_name))) == NULL)
        return NULL;

    dot_name->first_name = strdup(name1);
    dot_name->second_name = strdup(name2);
    dot_name->count = 1;

    return dot_name;
}

static void delete_dot_name(t_dot_name *dot_name)
{
    if (dot_name == NULL)
        return ;

    free(dot_name->first_name);
    free(dot_name->second_name);
    free(dot_name);
}

static int compare_dot_file(t_dot_name *dot_name, t_dot_name *ref)
{
    if (strcmp(dot_name->first_name, ref->first_name) != 0)
        return 1;
    return strcmp(dot_name->second_name, ref->second_name);
}

bool open_dot_file(char *filename)
{
    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
        return false;

    dprintf(fd, "digraph G {\n");

    return true;
}

void add_syscall_in_dot(t_proc *proc, int syscall)
{
    if (size_of_stack() <= 0)
        return ;

    unsigned long addr = front_addr_to_stack();
    char name[BUFF_SIZE];
    addr_to_fullname(proc, addr, name, BUFF_SIZE);

    t_dot_name *dot_name;
    if (name_list != NULL)
    {
        t_dot_name dot;
        dot.first_name = name;
        dot.second_name = tab_syscall[syscall].name;
        if ((dot_name = list_search_data(name_list, &dot, (cmp)&compare_dot_file)) != NULL)
        {
            ++dot_name->count;
            return ;
        }
    }

    dot_name = create_dot_name(name, tab_syscall[syscall].name);
    if (dot_name == NULL)
        return ;

    if (name_list == NULL)
        name_list = list_create(NULL);

    list_push_back(name_list, dot_name);

    if (tab_syscall[syscall].use == 0)
    {
        tab_syscall[syscall].use = 1;
        dprintf(fd, "\t\"%s\" [shape=rectangle,color=red];\n", tab_syscall[syscall].name);
    }
}

void add_call_in_dot(t_proc *proc, unsigned long addr)
{
    char name1[BUFF_SIZE];
    char name2[BUFF_SIZE];

    if (size_of_stack() <= 0)
        return ;

    unsigned long a = front_addr_to_stack();
    addr_to_fullname(proc, addr, name1, BUFF_SIZE);
    addr_to_fullname(proc, a, name2, BUFF_SIZE);

    t_dot_name *dot_name;
    if (name_list != NULL)
    {
        t_dot_name dot;
        dot.first_name = name2;
        dot.second_name = name1;
        if ((dot_name = list_search_data(name_list, &dot, (cmp)&compare_dot_file)) != NULL)
        {
            ++dot_name->count;
            return ;
        }
    }

    dot_name = create_dot_name(name2, name1);
    if (dot_name == NULL)
        return ;

    if (name_list == NULL)
        name_list = list_create(NULL);

    list_push_back(name_list, dot_name);
}

void close_dot_file()
{
    if (fd == -1 || name_list == NULL)
        return ;

    t_elm *itr;
    for (itr = list_begin(name_list); itr != list_end(name_list); list_inc(&itr))
    {
        t_dot_name *dot_name = itr->data;
        dprintf(fd, "\t\"%s\" -> \"%s\" [label=\"%d time%s\"];\n", dot_name->first_name, dot_name->second_name, dot_name->count, dot_name->count < 2 ? "" : "s");
    }

    list_delete(name_list, NULL, (freedata)&delete_dot_name);

    dprintf(fd, "}\n");
    close(fd);
}
