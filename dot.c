/*
** dot.c for ftrace in /home/leroy_v/Perso/ftrace
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Mon Mar 17 23:38:36 2014 vincent leroy
** Last update Wed Mar 19 19:30:42 2014 vincent leroy
*/

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "syscall.h"
#include "ftrace.h"
#include "stdlist.h"

typedef struct s_dot_name
{
    t_function *func1;
    t_function *func2;
    int count;
} t_dot_name;

static t_list *name_list;
static int fd = -1;

static t_dot_name* create_dot_name(t_function *func1, t_function *func2)
{
    t_dot_name *dot_name;

    if ((dot_name = calloc(1, sizeof(t_dot_name))) == NULL)
        return NULL;

    dot_name->func1 = func1;
    dot_name->func2 = func2;
    dot_name->count = 1;

    return dot_name;
}

static void delete_dot_name(t_dot_name *dot_name)
{
    if (dot_name == NULL)
        return ;

    free(dot_name);
}

static int compare_dot_file(t_dot_name *dot_name, t_dot_name *ref)
{
    if (strcmp(dot_name->func1->name, ref->func1->name) != 0)
        return 1;
    if (strcmp(dot_name->func2->name, ref->func2->name) != 0)
        return -1;
    return 0;
}

static int compare_list(t_list *list, char *library)
{
    return strcmp(list_user_data(list), library);
}

static void delete_list_library(t_list *list)
{
    if (list == NULL)
        return ;

    list_delete(list, &free, (freedata)&delete_dot_name);
}

static t_list* get_list_library(char *library)
{
    t_list *list;

    if (name_list == NULL)
    {
        name_list = list_create(NULL);
        list = list_create(strdup(library));
        list_push_back(name_list, list);
    }
    else
    {
        list = list_search_data(name_list, library, (cmp)&compare_list);
        if (list == NULL)
        {
            list = list_create(strdup(library));
            list_push_back(name_list, list);
        }
    }

    return list;
}

static void add_line_in_dot(t_function *func1, t_function *func2, char *library)
{
    t_list *list = get_list_library(library);
    t_dot_name *dot_name;
    t_dot_name dot = {func1, func2, 0};

    dot_name = list_search_data(list, &dot, (cmp)&compare_dot_file);
    if (dot_name != NULL)
    {
        ++dot_name->count;
        return ;
    }

    dot_name = create_dot_name(func1, func2);
    if (dot_name == NULL)
        return ;

    list_push_back(list, dot_name);
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
    t_function *func = addr_to_name(proc, addr);
    t_function *sys = syscall_to_name(syscall);

    add_line_in_dot(func, sys, "syscall");

    if (tab_syscall[syscall].use == 0)
    {
        tab_syscall[syscall].use = 1;
        dprintf(fd, "\t\"%s\" [shape=rectangle,color=red];\n", sys->name);
    }
}

void add_call_in_dot(t_proc *proc, unsigned long addr)
{
    if (size_of_stack() <= 0)
        return ;

    unsigned long a = front_addr_to_stack();
    t_function *func1 = addr_to_name(proc, a);
    char *lib = addr_to_file(proc, a);
    t_function *func2 = addr_to_name(proc, addr);

    add_line_in_dot(func1, func2, lib);
}

void close_dot_file()
{
    if (fd == -1 || name_list == NULL)
        return ;

    int nb = 0;
    t_elm *itr;
    for (itr = list_begin(name_list); itr != list_end(name_list); list_inc(&itr))
    {
        t_list *list_library = itr->data;
        char *library = list_user_data(list_library);

        dprintf(fd, "\n\tsubgraph cluster%d {\n", nb);
        dprintf(fd, "\t\tlabel = \"%s\";\n", library);
        dprintf(fd, "\t\tstyle = filled;\n");
        dprintf(fd, "\t\tcolor = grey;\n");
        dprintf(fd, "\t\tnode [style=filled,color=green3];\n");
        dprintf(fd, "\n");

        t_elm *itr2;
        for (itr2 = list_begin(list_library); itr2 != list_end(list_library); list_inc(&itr2))
        {
            t_dot_name *dot_name = itr2->data;

            dprintf(fd, "\t\t\"%s\" -> \"%s\"", dot_name->func1->name, dot_name->func2->name);
            if (dot_name->count > 1)
                dprintf(fd, " [label=\"%d times\"]", dot_name->count);
            dprintf(fd, ";\n");
        }

        dprintf(fd, "\t}\n");
        ++nb;
    }

    list_delete(name_list, NULL, (freedata)&delete_list_library);

    dprintf(fd, "}\n");
    close(fd);
}
