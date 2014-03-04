/*
** catch_signal.c for ftrace in /home/leroy_v/Perso/ftrace
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Sat Mar 01 15:15:50 2014 vincent leroy
** Last update Sat Mar 01 15:34:53 2014 vincent leroy
*/

#include <signal.h>
#include <string.h>

#include "ftrace.h"

static void inter_sig(int signum)
{
    switch (signum)
    {
        case SIGQUIT:
        case SIGINT:
            eprintf("Interruption\n");
            run = 0;
            break;
        default: break;
    }
}

void catch_signal()
{
    struct sigaction act;

    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = &inter_sig;

    sigaction(SIGINT, &act, NULL);
    sigaction(SIGQUIT, &act, NULL);

    act.sa_handler = SIG_IGN;

    sigaction(SIGTSTP, &act, NULL);
}
