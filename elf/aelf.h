/*
** elf.h for ftrace in /home/leroy_v/Perso/ftrace
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Tue Mar 04 21:37:27 2014 vincent leroy
** Last update Mon Mar 10 14:17:32 2014 vincent leroy
*/

#ifndef ELF_H_
# define ELF_H_

#include <gelf.h>
#include <stdbool.h>

#include "stdlist.h"

#ifndef eprintf
#include <stdio.h>

#define eprintf(...)    fprintf(stderr, __VA_ARGS__)
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
    Elf_Data    *dynamic;
} t_elf_data;

typedef struct s_elf
{
    int         fd;
    char        *filename;
    Elf         *elf;
    t_elf_data  data;
    t_list      *dep_list;
} t_elf;

typedef struct s_dep_file
{
    char    *filename;
    char    *pathname;
    t_elf   *elf;
} t_dep_file;

t_elf* open_elf_file(char *file, bool resolve_dependency);
void delete_elf(t_elf *elf);
bool parse_elf_file(t_elf *elf);

void dependency_in_elf(t_elf *elf, bool resolve_dependency);
void delete_dependency_list();
void delete_dep_file(t_dep_file *dep_file);

char* function_name_in_elf(t_elf *elf, unsigned long addr);

#endif /* !ELF_H_ */
