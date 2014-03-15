/*
** addr_to_name.c for ftrace in /home/leroy_v/Perso/ftrace
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Fri Mar 07 18:19:31 2014 vincent leroy
** Last update Wed Mar 12 15:57:16 2014 vincent leroy
*/

#include "ftrace.h"
#include "stdlist.h"
#include "aelf.h"

static t_list *elf_list = NULL;

static char* search_in_elf_list(unsigned long addr)
{
    t_elm *itr;
    char *name;

    for (itr = list_begin(elf_list); itr != list_end(elf_list); list_inc(&itr))
    {
        t_elf *elf = itr->data;
        if ((name = function_name_in_elf(elf, addr)) != NULL)
            return name;
    }

    return NULL;
}

char* addr_to_name(unsigned long addr)
{
    char *name;

    if (elf_list == NULL)
        elf_list = list_create(NULL);

    if ((name = search_in_elf_list(addr)) != NULL)
        return name;

    return NULL;
}

void delete_elf_list()
{
    if (elf_list == NULL)
        return ;

    list_delete(elf_list, NULL, (freedata)&delete_elf);
}
