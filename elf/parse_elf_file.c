/*
** parse_elf_file.c for elf in /home/leroy_v/Perso/ftrace/elf
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Tue Mar 04 21:51:25 2014 vincent leroy
** Last update Thu Mar 13 20:46:13 2014 vincent leroy
*/

#include <string.h>

#include "aelf.h"

static void fill_elf_data(t_elf *elf, Elf_Scn *scn, size_t shstrndx)
{
    GElf_Shdr shdr;
    char *name;

    if (gelf_getshdr(scn, &shdr) == NULL)
        return ;
    if ((name = elf_strptr(elf->elf, shstrndx, shdr.sh_name)) == NULL)
        return ;

    if (strcmp(name, ".strtab") == 0)
        elf->data.strtab = elf_getdata(scn, NULL);
    else if (strcmp(name, ".symtab") == 0)
        elf->data.symtab = elf_getdata(scn, NULL);
    else if (strcmp(name, ".dynstr") == 0)
        elf->data.dynstr = elf_getdata(scn, NULL);
    else if (strcmp(name, ".dynsym") == 0)
        elf->data.dynsym = elf_getdata(scn, NULL);
    else if (strcmp(name, ".rel.dyn") == 0)
        elf->data.rel_dyn = elf_getdata(scn, NULL);
    else if (strcmp(name, ".rel.plt") == 0)
        elf->data.rel_plt = elf_getdata(scn, NULL);
    else if (strcmp(name, ".rela.dyn") == 0)
        elf->data.rela_dyn = elf_getdata(scn, NULL);
    else if (strcmp(name, ".rela.plt") == 0)
        elf->data.rela_plt = elf_getdata(scn, NULL);
    else if (strcmp(name, ".plt") == 0)
    {
        elf->plt_begin = shdr.sh_addr;
        elf->plt_end = elf->plt_begin + shdr.sh_size;
    }
}

bool parse_elf_file(t_elf *elf)
{
    size_t shstrndx;
    Elf_Scn *scn = NULL;

    if (elf_getshdrstrndx(elf->elf, &shstrndx) == -1)
        return false;

    while ((scn = elf_nextscn(elf->elf, scn)) != NULL)
        fill_elf_data(elf, scn, shstrndx);

    if ((elf->function_map = list_create(NULL)) == NULL)
        return false;

    return true;
}
