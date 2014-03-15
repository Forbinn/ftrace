/*
** function_name_in_elf.c for elf in /home/leroy_v/Perso/ftrace/elf
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Wed Mar 05 12:26:21 2014 vincent leroy
** Last update Fri Mar 14 18:55:22 2014 vincent leroy
*/

#include <string.h>

#include "aelf.h"

//#define __TEST__

static char* name_in_strtab(Elf_Data *strtab, Elf_Data *symtab, unsigned long addr)
{
    GElf_Sym sym;
    int i;
    char *tab;

    if (strtab == NULL || symtab == NULL || strtab->d_buf == NULL)
        return NULL;

    for (i = 0; gelf_getsym(symtab, i, &sym) != NULL; ++i)
    {
#ifndef __TEST__
        if (sym.st_value != addr)
            continue;
#else
        (void)addr;
#endif

        if (GELF_ST_TYPE(sym.st_info) != STT_FUNC)
            continue;

        tab = strtab->d_buf + sym.st_name;
        if (tab[0] == '\0' || sym.st_value == 0)
            continue;

#ifndef __TEST__
        return strdup(tab);
#else
        eprintf("strtab => %#lx (%p) => [%s]\n", sym.st_value, tab, tab);
#endif
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

#ifndef __TEST__
        if (addr != offset)
            continue;
#else
        (void)addr;
#endif

        if (gelf_getsym(dynsym, GELF_R_SYM(word), &sym) != NULL)
        {
            if (GELF_ST_TYPE(sym.st_info) != STT_FUNC)
                continue;

            tab = dynstr->d_buf + sym.st_name;
#ifndef __TEST__
            return strdup(tab);
#else
            eprintf("dynstr => %#lx (%p) => [%s]\n", offset, tab, tab);
#endif
        }
    }

    return NULL;
}

static int compare_function_and_addr(t_function *func, unsigned long *addr)
{
    return func->addr == *addr ? 0 : 1;
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
    t_function *func;
    char *name;

    if ((func = list_search_data(elf->function_map, &addr, (cmp)&compare_function_and_addr)) != NULL)
        return func->name;

    if ((name = name_in_strtab(elf->data.strtab, elf->data.symtab, addr)) == NULL)
    {
        int i;
        for (i = 0; !relocation[i].end; ++i)
            if ((name = name_in_relocation(elf->data.dynsym, elf->data.dynstr, relocation[i].relocation, relocation[i].is_rela, addr)) != NULL)
                break;
    }

    if (name == NULL)
        return NULL;

    if ((func = calloc(1, sizeof(t_function))) == NULL)
        return NULL;

    func->addr = addr;
    func->name = name;

    list_push_back(elf->function_map, func);

    return name;
}
