/*
** functionNameInElf.c for elf in /home/leroy_v/Perso/ftrace/elf
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Wed Mar 05 12:26:21 2014 vincent leroy
** Last update Sat Mar 08 15:39:44 2014 vincent leroy
*/

#include "aelf.h"

char* name_in_strtab(Elf_Data *strtab, Elf_Data *symtab, unsigned long addr)
{
    GElf_Sym sym;
    int i;
    char *tab;

    if (strtab == NULL || symtab == NULL || strtab->d_buf == NULL)
        return NULL;

    (void)addr;
    for (i = 0; gelf_getsym(symtab, i, &sym) != NULL; ++i)
    {
        //if (sym.st_value != addr)
        //    continue;

        tab = strtab->d_buf + sym.st_name;
        if (tab[0] == '\0')
            continue;

        eprintf("%08lx = [%s]\n", sym.st_value, tab);
    }

    return NULL;
}

char* name_in_relocation(Elf_Data *dynsym, Elf_Data *dynstr, Elf_Data *relocation, const bool is_rela, unsigned long addr)
{
    GElf_Sym sym;
    GElf_Rel rel;
    GElf_Rela rela;
    int i;
    char *tab;
    void *ptr;

    if (dynsym == NULL || dynstr == NULL || relocation == NULL)
        return NULL;

    (void)addr;
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

        //if (addr != offset)
        //    continue;

        if (gelf_getsym(dynsym, GELF_R_SYM(word), &sym) != NULL)
        {
            tab = dynstr->d_buf + sym.st_name;
            eprintf("%08lx = [%s]\n", offset, tab);
        }
    }

    return NULL;
}

char* function_name_in_elf(t_elf *elf, unsigned long addr)
{
    (void)addr;
    (void)elf;
    /*eprintf("strtab:\n");
    name_in_strtab(elf->data.strtab, elf->data.symtab, addr);
    eprintf("rel.dyn:\n");
    name_in_relocation(elf->data.dynsym, elf->data.dynstr, elf->data.rel_dyn, false, addr);
    eprintf("rel.plt:\n");
    name_in_relocation(elf->data.dynsym, elf->data.dynstr, elf->data.rel_plt, false, addr);
    eprintf("rela.dyn:\n");
    name_in_relocation(elf->data.dynsym, elf->data.dynstr, elf->data.rela_dyn, true, addr);
    eprintf("rela.plt:\n");
    name_in_relocation(elf->data.dynsym, elf->data.dynstr, elf->data.rela_plt, true, addr);*/
    return NULL;
}
