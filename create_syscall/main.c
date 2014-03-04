/*
 ** main.c for tmp in /home/leroy_v/Epitech/projet_tek2/tmp
 **
 ** Made by vincent leroy
 ** Login  <leroy_v@epitech.eu>
 **
 ** Started on  Mon Apr 29 13:46:13 2013 vincent leroy
 ** Last update Sat Mar 01 15:08:11 2014 vincent leroy
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

const char *unistd_64[] = {
    "/usr/include/asm/unistd_64.h",
    "/usr/include/x86_64-linux-gnu/asm/unistd_64.h",
    NULL
};

const char *unistd_32[] = {
    "/usr/include/asm/unistd_32.h",
    NULL
};

void aff_struct(int fdOut)
{
    dprintf(fdOut, "typedef struct s_syscall\n");
    dprintf(fdOut, "{\n");
    dprintf(fdOut, "    int     op_code;\n");
    dprintf(fdOut, "    int     use;\n");
    dprintf(fdOut, "    char    *name;\n");
    dprintf(fdOut, "} t_syscall;\n\n");

    dprintf(fdOut, "t_syscall tab_syscall[] =\n");
    dprintf(fdOut, "{\n");
}

void aff_begin(int fdOut)
{
    dprintf(fdOut, "#ifndef SYSCALL_H_\n");
    dprintf(fdOut, "# define SYSCALL_H_\n\n");

    aff_struct(fdOut);
}

void aff_end(int fdOut, int nb_syscall)
{
    dprintf(fdOut, "};\n\n");

    dprintf(fdOut, "#define NB_SYSCALL\t%d\n\n", nb_syscall);

    dprintf(fdOut, "#endif /* !SYSCALL_H_ */\n");
}

FILE* open_unistd_32()
{
    FILE *fd;
    int i;

    for (i = 0; unistd_32[i] != NULL; ++i)
        if ((fd = fopen(unistd_32[i], "r")) != NULL)
            return fd;
    return NULL;
}

FILE* open_unistd_64()
{
    FILE *fd;
    int i;

    for (i = 0; unistd_64[i] != NULL; ++i)
        if ((fd = fopen(unistd_64[i], "r")) != NULL)
            return fd;
    return NULL;
}

int main(int ac, char **av)
{
    FILE *fd;
    int fdOut;
    char buff[4096];
    char *ptr;
    char *name;
    char *nb;
    int nb_syscall = 0;
    int architecture = 1;
    int i;

    for (i = 1; i < ac; ++i)
    {
        if (strcmp(av[1], "-m32") == 0)
            architecture = 0;
        else if (strcmp(av[1], "-m64") == 0)
            architecture = 1;
        else
        {
            fprintf(stderr, "Unknow option '%s'\n", av[i]);
            fprintf(stderr, "Usage: %s [-m32 | -m64]\n", av[0]);
            return 1;
        }
    }

    if (architecture == 0)
        fd = open_unistd_32();
    else
        fd = open_unistd_64();

    if (fd == NULL)
    {
        fprintf(stderr, "Unable to open file unistd.h\n");
        return 1;
    }
    if ((fdOut = open("syscall.h", O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
    {
        fclose(fd);
        fprintf(stderr, "Unable to open/create file syscall.h: %s\n", strerror(errno));
        return 1;
    }

    aff_begin(fdOut);

    while (fgets(buff, 4095, fd) != NULL)
    {
        if (strncmp(buff, "#define __NR_", 13) == 0)
        {
            if ((ptr = strrchr(buff, '\n')) != NULL)
                ptr[0] = '\0';
            ptr = buff + 13;
            name = strtok(ptr, " \t");
            nb = strtok(NULL, " \t");
            dprintf(fdOut, "  {%s, 0, \"%s\"},\n", nb, name);
            ++nb_syscall;
        }
    }

    aff_end(fdOut, nb_syscall);

    fclose(fd);
    close(fdOut);
    return 0;
}
