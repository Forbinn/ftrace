/*
** main.c for elf in /home/leroy_v/Perso/ftrace/elf
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Tue Mar 04 21:40:34 2014 vincent leroy
** Last update Wed Mar 12 12:11:02 2014 vincent leroy
*/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "aelf.h"

int main(int ac, char **av)
{
    t_elf *elf;

    if (ac != 2)
    {
        fprintf(stderr, "Usage: %s file.so\n", av[0]);
        return 1;
    }

    if ((elf = open_elf_file(av[1])) == NULL)
    {
        fprintf(stderr, "Unable to open elf file\n");
        return 1;
    }

    function_name_in_elf(elf, 0);

    delete_elf(elf);

    return 0;
}
