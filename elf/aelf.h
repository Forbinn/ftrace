/*
** aelf.h for ftrace in /home/leroy_v/Perso/ftrace
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Tue Mar 04 21:37:27 2014 vincent leroy
** Last update Thu Mar 13 20:42:49 2014 vincent leroy
*/

#ifndef AELF_H_
# define AELF_H_

#include <gelf.h>
#include <stdbool.h>

#include "stdlist.h"

#ifndef eprintf
#include <stdio.h>

#define eprintf(...)    fprintf(stdout, __VA_ARGS__)
#endif

typedef struct s_elf_data
{
    Elf_Data    *strtab;
    Elf_Data    *symtab;
    Elf_Data    *dynstr;
    Elf_Data    *dynsym;
    Elf_Data    *rel_dyn;
    Elf_Data    *rel_plt;
    Elf_Data    *rela_dyn;
    Elf_Data    *rela_plt;
} t_elf_data;

typedef struct s_elf
{
    int             fd;
    char            *filename;
    Elf             *elf;
    t_elf_data      data;
    unsigned long   plt_begin;
    unsigned long   plt_end;
    t_list          *function_map;
} t_elf;

typedef struct s_function
{
    unsigned long   addr;
    char            *name;
} t_function;

t_elf* open_elf_file(char *file);
void delete_elf(t_elf *elf);
bool parse_elf_file(t_elf *elf);

char* function_name_in_elf(t_elf *elf, unsigned long addr);

#endif /* !AELF_H_ */
