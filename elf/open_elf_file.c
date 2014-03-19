/*
** open_elf_file.c for elf in /home/leroy_v/Perso/ftrace/elf
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Tue Mar 04 21:41:54 2014 vincent leroy
** Last update Wed Mar 19 14:17:06 2014 vincent leroy
*/

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "aelf.h"

t_elf* open_elf_file(char *file)
{
    t_elf *elf;

    if (elf_version(EV_CURRENT) == EV_NONE)
        return NULL;

    if ((elf = calloc(1, sizeof(t_elf))) != NULL)
    {
        elf->filename = strdup(file);
        if ((elf->fd = open(file, O_RDONLY)) != -1)
        {
            if ((elf->elf = elf_begin(elf->fd, ELF_C_READ, NULL)) != NULL)
            {
                if (elf_kind(elf->elf) == ELF_K_ELF)
                    if (parse_elf_file(elf))
                        return elf;

                elf_end(elf->elf);
            }
            close(elf->fd);
        }
        free(elf->filename);
        free(elf);
    }

    return NULL;
}

void delete_elf(t_elf *elf)
{
    if (elf == NULL)
        return ;

    elf_end(elf->elf);
    close(elf->fd);
    free(elf->filename);
    free(elf);
}
