##
## Makefile
##
## Made by vincent leroy
## Login   <leroy_v@epitech.eu>
##
## Started on  Fri Feb 28 12:45:46 2014 vincent leroy
## Last update Mon Mar 17 17:59:17 2014 vincent leroy
##

SRC			= main.c \
			  exec_program.c \
			  exec_ftrace.c \
			  catch_signal.c \
			  check_syscall.c \
			  check_call.c \
			  stack.c \
			  check_ret.c \
			  check_fp_call.c \
			  check_ind_fp_call.c \
			  check_off_fp_call.c \
			  check_2_reg_call.c \
			  check_off_2_reg_call.c \
			  addr_to_name.c \
			  read_proc_maps.c \

LIST		= stdlist

ELF			= elf

SYSCALL		= create_syscall

CFLAGS		= -Wall -Wextra -Werror
CFLAGS		+= -I$(LIST)
CFLAGS		+= -I$(ELF)

LDFLAGS		= -L$(LIST) -llist
LDFLAGS		+= -L$(ELF) -laelf
LDFLAGS		+= -Wl,-rpath=$(LIST),-rpath=$(ELF)
LDFLAGS		+= -lelf

NAME		= ftrace
NAME_DEBUG	= $(NAME).debug

OBJ			= $(SRC:.c=.o)
OBJ_DEBUG	= $(SRC:.c=.debug.o)

RM			= rm -f

CC			= gcc

MAKE		= make -C

all: elf list syscall $(NAME)

elf:
	$(MAKE) $(ELF) shared

list:
	$(MAKE) $(LIST) shared

syscall:
	$(MAKE) $(SYSCALL)
	$(SYSCALL)/$(SYSCALL) -m64

$(NAME): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $(NAME)

clean:
	$(MAKE) $(ELF) clean
	$(MAKE) $(LIST) clean
	$(MAKE) $(SYSCALL) clean
	$(RM) $(OBJ) $(OBJ_DEBUG) *.swp *~ *#

fclean: clean
	$(RM) $(NAME) $(NAME_DEBUG)

re: fclean all

debug: CFLAGS += -ggdb3
debug: $(OBJ_DEBUG)
	$(CC) $(OBJ_DEBUG) $(LDFLAGS) -o $(NAME_DEBUG)

%.debug.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all elf list syscall clean fclean re debug
