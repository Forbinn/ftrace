/*
** stack.c for ftrace in /home/leroy_v/Perso/ftrace
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Sat Mar 01 21:08:11 2014 vincent leroy
** Last update Wed Mar 12 18:06:55 2014 vincent leroy
*/

#include "ftrace.h"
#include "stdlist.h"

static t_list *stack;

void push_addr_to_stack(unsigned long addr)
{
    if (stack == NULL)
        stack = list_create(NULL);
    list_push_front(stack, (void*)addr);
}

void pop_addr_to_stack()
{
    list_pop_front(stack, NULL);
}

unsigned long front_addr_to_stack()
{
    return (unsigned long)list_front(stack);
}

void delete_stack()
{
    if (stack == NULL)
        return ;
    eprintf("Stack size at the end: %d\n", list_size(stack));
    list_delete(stack, NULL, NULL);
}

int size_of_stack()
{
    if (stack == NULL)
        return 0;

    return list_size(stack);
}
