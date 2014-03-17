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

void aff_struct(int fdOuth, int fdOutc)
{
    dprintf(fdOuth, "typedef struct s_syscall\n");
    dprintf(fdOuth, "{\n");
    dprintf(fdOuth, "    int     op_code;\n");
    dprintf(fdOuth, "    int     use;\n");
    dprintf(fdOuth, "    char    *name;\n");
    dprintf(fdOuth, "} t_syscall;\n\n");

    dprintf(fdOuth, "extern t_syscall tab_syscall[];\n\n");


    dprintf(fdOutc, "#include \"syscall.h\"\n\n");
    dprintf(fdOutc, "t_syscall tab_syscall[] =\n");
    dprintf(fdOutc, "{\n");
}

void aff_begin(int fdOuth, int fdOutc)
{
    dprintf(fdOuth, "#ifndef SYSCALL_H_\n");
    dprintf(fdOuth, "# define SYSCALL_H_\n\n");

    aff_struct(fdOuth, fdOutc);
}

void aff_end(int fdOuth, int fdOutc, int nb_syscall)
{
    dprintf(fdOutc, "};\n");

    dprintf(fdOuth, "#define NB_SYSCALL\t%d\n\n", nb_syscall);

    dprintf(fdOuth, "#endif /* !SYSCALL_H_ */\n");
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
    int fdOuth;
    int fdOutc;
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
    if ((fdOuth = open("syscall.h", O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
    {
        fclose(fd);
        fprintf(stderr, "Unable to open/create file syscall.h: %m\n");
        return 1;
    }
    if ((fdOutc = open("syscall.c", O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
    {
        fclose(fd);
        close(fdOuth);
        fprintf(stderr, "Unable to open/create file syscall.c: %m\n");
    }

    aff_begin(fdOuth, fdOutc);

    while (fgets(buff, 4095, fd) != NULL)
    {
        if (strncmp(buff, "#define __NR_", 13) == 0)
        {
            if ((ptr = strrchr(buff, '\n')) != NULL)
                ptr[0] = '\0';
            ptr = buff + 13;
            name = strtok(ptr, " \t");
            nb = strtok(NULL, " \t");
            dprintf(fdOutc, "  {%s, 0, \"%s\"},\n", nb, name);
            ++nb_syscall;
        }
    }

    aff_end(fdOuth, fdOutc, nb_syscall);

    fclose(fd);
    close(fdOuth);
    close(fdOutc);
    return 0;
}
