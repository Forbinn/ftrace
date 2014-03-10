/*
** dependency_in_elf.c for elf in /home/leroy_v/Perso/ftrace/elf
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Sat Mar 08 13:15:12 2014 vincent leroy
** Last update Mon Mar 10 17:03:42 2014 vincent leroy
*/

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include "aelf.h"

static t_list *dependency_list = NULL;

static bool check_this_path(t_dep_file *dep_file, char *path)
{
    if (dep_file->pathname != NULL)
        return true;

    char pathname[4096];

    if (path[strlen(path) - 1] == '/')
        snprintf(pathname, 4096, "%s%s", path, dep_file->filename);
    else
        snprintf(pathname, 4096, "%s/%s", path, dep_file->filename);

    if (access(pathname, X_OK) == -1)
        return false;

    dep_file->pathname = strdup(pathname);
    return true;
}

/*
 * Search path of shared library depency in this order:
 *  - DT_RPATH if DT_RUNPATH not present
 *  - LD_LIBRARY_PATH except if executable is set-uid or set-gid
 *  - DT_RUNPATH
 *  - /etc/ld.so.cache (unusable : compiled file)
 *  - /lib /usr/lib
 */
static bool resolve_path_dependency(t_dep_file *dep_file, char *rpath, char *runpath, int elfclass)
{
    static char *hardcoded_path64[] = {
        "/lib64",
        "/usr/lib64",
        NULL
    };
    static char *hardcoded_path32[] = {
        "/lib",
        "/usr/lib",
        NULL
    };

    if (dep_file == NULL)
        return false;

    if (rpath != NULL && runpath == NULL)
    {
        char *token;
        while ((token = strtok(rpath, ":")) != NULL)
        {
            if (check_this_path(dep_file, token))
                return true;
            rpath = NULL;
        }
    }

    char *ld_library_path;
    if (getuid() == geteuid() && getgid() == getegid())
        if ((ld_library_path = getenv("LD_LIBRARY_PATH")) != NULL)
            if (check_this_path(dep_file, ld_library_path))
                return true;

    if (runpath != NULL)
    {
        char *token;
        while ((token = strtok(runpath, ":")) != NULL)
        {
            if (check_this_path(dep_file, token))
                return true;
            runpath = NULL;
        }
    }

    int i;
    if (elfclass == ELFCLASS32)
    {
        for (i = 0; hardcoded_path32[i] != NULL; ++i)
            if (check_this_path(dep_file, hardcoded_path32[i]))
                return true;
    }
    else
    {
        for (i = 0; hardcoded_path64[i] != NULL; ++i)
            if (check_this_path(dep_file, hardcoded_path64[i]))
                return true;
    }

    return false;
}

static int compare_filename_dep_file(t_dep_file *dep_file, char *ref)
{
    return strcmp(dep_file->filename, ref);
}

static void add_new_dependency(char *filename, t_elf *elf)
{
    t_dep_file *dep_file;

    if (dependency_list != NULL)
    {
        int pos = list_search_data(dependency_list, filename, (cmp)&compare_filename_dep_file);
        if (pos != -1)
        {
            list_push_back(elf->dep_list, list_get_data(dependency_list, pos));
            return ;
        }
    }
    else if ((dependency_list = list_create(NULL)) == NULL)
        return ;

    if ((dep_file = calloc(1, sizeof(t_dep_file))) == NULL)
        return ;

    dep_file->filename = filename;
    list_push_back(dependency_list, dep_file);
    list_push_back(elf->dep_list, dep_file);
}

void dependency_in_elf(t_elf *elf, bool resolve_dependency)
{
    if (elf->data.dynamic == NULL)
        return ;
    if (elf->dep_list == NULL)
        if ((elf->dep_list = list_create(NULL)) == NULL)
            return ;

    int i;
    GElf_Dyn dyn;
    char *rpath = NULL;
    char *runpath = NULL;

    for (i = 0; gelf_getdyn(elf->data.dynamic, i, &dyn) != NULL; ++i)
    {
        if (dyn.d_tag == DT_NEEDED && elf->data.dynstr != NULL)
            add_new_dependency(elf->data.dynstr->d_buf + dyn.d_un.d_val, elf);
        else if (dyn.d_tag == DT_RPATH && elf->data.dynstr != NULL)
            rpath = elf->data.dynstr->d_buf + dyn.d_un.d_val;
        else if (dyn.d_tag == DT_RUNPATH && elf->data.strtab != NULL)
            runpath = elf->data.strtab->d_buf + dyn.d_un.d_val;
    }

    if (dependency_list == NULL)
        return ;

    t_elm *elm = dependency_list->first;
    do
    {
        t_dep_file *dep_file = elm->data;
        if (dep_file->pathname == NULL)
        {
            if (resolve_path_dependency(dep_file, rpath, runpath, gelf_getclass(elf->elf)))
                if (resolve_dependency)
                    dep_file->elf = open_elf_file(dep_file->pathname, resolve_dependency);
        }
        elm = elm->next;
    }
    while (elm != dependency_list->first);
}

void delete_dependency_list()
{
    if (dependency_list == NULL)
        return ;

    list_delete(dependency_list, NULL, (freedata)&delete_dep_file);
}

void delete_dep_file(t_dep_file *dep_file)
{
    if (dep_file == NULL)
        return ;

    free(dep_file->pathname);
    delete_elf(dep_file->elf);
    free(dep_file);
}
