/*
** main.c for ftrace in /home/leroy_v/Perso/ftrace
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Fri Feb 28 12:52:43 2014 vincent leroy
** Last update Sat Mar 01 21:26:07 2014 vincent leroy
*/

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>

#include "ftrace.h"

static void NORETURN usage(const char *progname)
{
    eprintf("Usage: %s [progname [argument]... | -p pid]\n", progname);
    exit(1);
}

static bool get_progname_by_pid(t_option *opt)
{
    char buff[4096];
    size_t size;

    snprintf(buff, 4096, "/proc/%d/exe", opt->pid);
    if ((size = readlink(buff, buff, 4096)) == (size_t)-1)
    {
        opt->progname = NULL;
        opt->pathprogname = NULL;
        eprintf("Unable to read program name: %m\n");
        return true;
    }
    buff[size] = '\0';

    opt->pathprogname = strdup(buff);
    opt->progname = strdup(basename(buff));

    return true;
}

static char* get_path()
{
    char *path = getenv("PATH");

    if (path != NULL)
        return strdup(path);

    size_t n = confstr(_CS_PATH, NULL, 0);
    if (n == 0)
        return NULL;
    if ((path = malloc(n)) == NULL)
        return NULL;
    if (confstr(_CS_PATH, path, n) == 0)
    {
        free(path);
        return NULL;
    }

    return path;
}

static bool get_pathprogname(t_option *opt)
{
    if (access(opt->progname, X_OK) == 0)
    {
        opt->pathprogname = opt->progname;
        opt->progname = strdup(basename(opt->pathprogname));
        return true;
    }

    char *path = get_path();

    if (path == NULL)
    {
        opt->pathprogname = strdup(opt->progname);
        eprintf("Unable to get PATH: %m\n");
        return true;
    }

    char *token;
    char *save = path;
    while ((token = strtok(path, ":")) != NULL)
    {
        char buff[4096];

        snprintf(buff, 4096, "%s/%s", token, opt->progname);
        if (access(buff, X_OK) != -1)
        {
            opt->pathprogname = strdup(buff);
            break;
        }
        path = NULL;
    }

    free(save);
    if (opt->pathprogname == NULL)
        eprintf("Program not found\n");
    return opt->pathprogname != NULL;
}

static bool parse_option(int ac, char **av, t_option *opt)
{
    if (ac < 2)
        return false;

    if (strcmp(av[1], "-p") == 0)
    {
        int i;
        for (i = 0; av[2][i] != '\0'; ++i)
            if (!isdigit(av[2][i]))
                return false;

        opt->pid = atoi(av[2]);
        return get_progname_by_pid(opt);
    }

    opt->progname = strdup(av[1]);
    if (!get_pathprogname(opt))
        return false;
    if ((opt->argument = calloc(ac, sizeof(char*))) == NULL)
        return false;
    int i;
    for (i = 1; i < ac; ++i)
        opt->argument[i - 1] = strdup(av[i]);
    return true;
}

int main(int ac, char **av)
{
    t_option opt = {NULL, NULL, NULL, 0};

    if (!parse_option(ac, av, &opt))
        usage(av[0]);

    exec_program(&opt);

    free(opt.pathprogname);
    free(opt.progname);
    if (opt.argument != NULL)
    {
        int i;
        for (i = 0; opt.argument[i] != NULL; ++i)
            free(opt.argument[i]);
        free(opt.argument);
    }
    delete_stack();

    return 0;
}
