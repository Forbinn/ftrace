/*
** read_proc_maps.h for ftrace in /home/leroy_v/Perso/ftrace
**
** Made by vincent leroy
** Login  <leroy_v@epitech.eu>
**
** Started on  Sat Mar 15 16:20:47 2014 vincent leroy
** Last update Mon Mar 17 12:44:58 2014 vincent leroy
*/

#ifndef READ_PROC_MAPS_H_
# define READ_PROC_MAPS_H_

#include "aelf.h"
#include "stdlist.h"

#ifndef BUFF_SIZE
#define BUFF_SIZE       1024
#endif

#define RIGHT_READ      0
#define RIGHT_WRITE     1
#define RIGHT_EXECUTE   2
#define RIGHT_PRIVACY   3
#define NB_RIGHT        4

#define PRIVATE         'p'
#define SHARED          's'

typedef struct s_shared_elf
{
    char            *filename;
    unsigned long   begin_addr;
    unsigned long   end_addr;
    unsigned long   offset;
    char            right[NB_RIGHT];
    t_elf           *elf;
} t_shared_elf;

typedef struct s_proc
{
    pid_t   pid;
    t_list  *elf_list;
} t_proc;

bool read_executable(t_proc *proc, char *pathname);
bool read_proc_maps(t_proc *proc);
void delete_proc(t_proc *proc);
char* addr_to_file(t_proc *proc, unsigned long addr);

#endif /* !READ_PROC_MAPS_H_ */
