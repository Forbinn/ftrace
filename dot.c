/*
** dot.c for ftrace in /home/leroy_v/Perso/ftrace
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Mon Mar 17 23:38:36 2014 vincent leroy
** Last update Tue Mar 18 20:06:23 2014 vincent leroy
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
    if (strcmp(dot_name->second_name, ref->second_name) != 0)
        return -1;
    return 0;
}

static void addr_to_string(unsigned long addr, char *buff, int size, char *n)
{
    if (n == NULL)
        snprintf(buff, size, "%#016lx", addr);
    else
        strncpy(buff, n, size);
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

static void add_line_in_dot(char *name1, char *name2, char *library)
{
    t_list *list = get_list_library(library);
    t_dot_name *dot_name;
    t_dot_name dot = {name1, name2, 0};

    dot_name = list_search_data(list, &dot, (cmp)&compare_dot_file);
    if (dot_name != NULL)
    {
        ++dot_name->count;
        return ;
    }

    dot_name = create_dot_name(name1, name2);
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
    char name[BUFF_SIZE];
    char sys[BUFF_SIZE];
    char *tmp = addr_to_name(proc, addr);
    addr_to_string(addr, name, BUFF_SIZE, tmp);

    snprintf(sys, BUFF_SIZE, "%s@syscall", tab_syscall[syscall].name);
    add_line_in_dot(name, sys, "syscall");

    if (tab_syscall[syscall].use == 0)
    {
        tab_syscall[syscall].use = 1;
        dprintf(fd, "\t\"%s\" [shape=rectangle,color=red];\n", sys);
    }
}

void add_call_in_dot(t_proc *proc, unsigned long addr)
{
    char name1[BUFF_SIZE];
    char name2[BUFF_SIZE];
    char lib[BUFF_SIZE];

    if (size_of_stack() <= 0)
        return ;

    unsigned long a = front_addr_to_stack();
    char *tmp = addr_to_name(proc, a);
    addr_to_string(a, name1, BUFF_SIZE, tmp);
    tmp = addr_to_file(proc, a);
    addr_to_string(a, lib, BUFF_SIZE, tmp);
    tmp = addr_to_name(proc, addr);
    addr_to_string(addr, name2, BUFF_SIZE, tmp);

    add_line_in_dot(name1, name2, lib);
}

void close_dot_file()
{
    if (fd == -1 || name_list == NULL)
        return ;

    int nb = 0;
    t_elm *itr;
    for (itr = list_begin(name_list); itr != list_end(name_list); list_inc(&itr), ++nb)
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
            dprintf(fd, "\t\t\"%s\" -> \"%s\" [label=\"%d time%s\"];\n", dot_name->first_name, dot_name->second_name, dot_name->count, dot_name->count < 2 ? "" : "s");
        }

        dprintf(fd, "\t}\n");
    }

    list_delete(name_list, NULL, (freedata)&delete_list_library);

    dprintf(fd, "}\n");
    close(fd);
}
