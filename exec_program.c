/*
** exec_program.c for ftrace in /home/leroy_v/Perso/ftrace
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Fri Feb 28 17:05:46 2014 vincent leroy
** Last update Tue Mar 18 13:37:14 2014 vincent leroy
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <signal.h>
#include <sys/wait.h>

#include "ftrace.h"

extern char **environ;

static bool attach_to_prog(t_option *opt)
{
    if (ptrace(PTRACE_ATTACH, opt->pid, NULL, NULL) == -1)
    {
        eprintf("Unable to attach the process ");
        if (opt->progname != NULL)
            eprintf("'%s'", opt->progname);
        else
            eprintf("%d", opt->pid);
        eprintf(" %m\n");
        return false;
    }

    return true;
}

static bool detach_to_prog(t_option *opt)
{
    if (ptrace(PTRACE_DETACH, opt->pid, NULL, NULL) == -1)
    {
        eprintf("Unable to detach the process ");
        if (opt->progname != NULL)
            eprintf("'%s'", opt->progname);
        else
            eprintf("%d", opt->pid);
        eprintf(" %m\n");
        return false;
    }

    return true;
}

static bool wait_execve(pid_t pid)
{
    waitpid(pid, NULL, WUNTRACED);

    if (ptrace(PTRACE_SYSCALL, pid, NULL, NULL) == -1)
        return false;

    waitpid(pid, NULL, WUNTRACED);

    if (ptrace(PTRACE_SYSCALL, pid, NULL, NULL) == -1)
        return false;

    return true;
}

bool exec_program(t_option *opt)
{
    if (opt->pid > 0)
    {
        if (!attach_to_prog(opt))
            return false;
        if (!exec_ftrace(opt))
            return false;
        if (!detach_to_prog(opt))
            return false;
        return true;
    }

    if ((opt->pid = fork()) == -1)
    {
        eprintf("Unable to fork: %m\n");
        return false;
    }
    else if (opt->pid == 0)
    {
        if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1)
        {
            eprintf("Unable to trace program '%s': %m\n", opt->progname);
            exit(1);
        }

        kill(getpid(), SIGSTOP);
        execve(opt->pathprogname, opt->argument, environ);

        eprintf("Unable to start program: %m\n");
        exit(1);
    }

    usleep(1000); // Needed for wait the child to start
    ptrace(PTRACE_SETOPTIONS, opt->pid, NULL, PTRACE_O_EXITKILL);
    if (!wait_execve(opt->pid))
    {
        eprintf("Error when waiting execve: %m\n");
        return false;
    }
    return exec_ftrace(opt);
}
