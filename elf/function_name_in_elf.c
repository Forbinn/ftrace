/*
** function_name_in_elf.c for elf in /home/leroy_v/Perso/ftrace/elf
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Wed Mar 05 12:26:21 2014 vincent leroy
** Last update Wed Mar 19 14:18:06 2014 vincent leroy
*/

#include <string.h>

#include "aelf.h"

static char* name_in_strtab(Elf_Data *strtab, Elf_Data *symtab, unsigned long addr)
{
    GElf_Sym sym;
    int i;
    char *tab;

    if (strtab == NULL || symtab == NULL || strtab->d_buf == NULL)
        return NULL;

    for (i = 0; gelf_getsym(symtab, i, &sym) != NULL; ++i)
    {
        if (sym.st_value != addr)
            continue;

        if (GELF_ST_TYPE(sym.st_info) != STT_FUNC)
            continue;

        tab = strtab->d_buf + sym.st_name;
        if (tab[0] == '\0' || sym.st_value == 0)
            continue;

        return strdup(tab);
    }

    return NULL;
}

static char* name_in_relocation(Elf_Data *dynsym, Elf_Data *dynstr, Elf_Data *relocation, const bool is_rela, unsigned long addr)
{
    GElf_Sym sym;
    GElf_Rel rel;
    GElf_Rela rela;
    int i;
    char *tab;
    void *ptr;

    if (dynsym == NULL || dynstr == NULL || relocation == NULL)
        return NULL;

    for (i = 0; ; ++i)
    {
        if (is_rela)
            ptr = gelf_getrela(relocation, i, &rela);
        else
            ptr = gelf_getrel(relocation, i, &rel);
        if (ptr == NULL)
            break;

        GElf_Addr offset = (is_rela ? rela.r_offset : rel.r_offset);
        Elf64_Xword word = (is_rela ? rela.r_info : rel.r_info);

        if (addr != offset)
            continue;

        if (gelf_getsym(dynsym, GELF_R_SYM(word), &sym) != NULL)
        {
            if (GELF_ST_TYPE(sym.st_info) != STT_FUNC)
                continue;

            tab = dynstr->d_buf + sym.st_name;
            return strdup(tab);
        }
    }

    return NULL;
}

char* function_name_in_elf(t_elf *elf, unsigned long addr)
{
    struct s_relocation
    {
        Elf_Data *relocation;
        bool is_rela;
        bool end;
    };
    struct s_relocation relocation[] = {
        {elf->data.rel_dyn, false, false},
        {elf->data.rel_plt, false, false},
        {elf->data.rela_dyn, true, false},
        {elf->data.rela_plt, true, false},
        {NULL, false, true}
    };
    char *name;

    if ((name = name_in_strtab(elf->data.strtab, elf->data.symtab, addr)) == NULL)
    {
        int i;
        for (i = 0; !relocation[i].end; ++i)
            if ((name = name_in_relocation(elf->data.dynsym, elf->data.dynstr, relocation[i].relocation, relocation[i].is_rela, addr)) != NULL)
                break;
    }

    return name;
}
