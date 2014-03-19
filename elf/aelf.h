/*
** aelf.h for ftrace in /home/leroy_v/Perso/ftrace
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Tue Mar 04 21:37:27 2014 vincent leroy
** Last update Wed Mar 19 14:17:09 2014 vincent leroy
*/

#ifndef AELF_H_
# define AELF_H_

#include <gelf.h>
#include <stdbool.h>

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
    unsigned long   file_begin;
    unsigned long   file_end;
    unsigned long   plt_begin;
    unsigned long   plt_end;
} t_elf;

t_elf* open_elf_file(char *file);
void delete_elf(t_elf *elf);
bool parse_elf_file(t_elf *elf);

char* function_name_in_elf(t_elf *elf, unsigned long addr);

#endif /* !AELF_H_ */
