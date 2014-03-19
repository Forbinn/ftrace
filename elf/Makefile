##
## Makefile
##
## Made by vincent leroy
## Login   <leroy_v@epitech.eu>
##
## Started on  Tue Mar 04 21:40:15 2014 vincent leroy
## Last update Wed Mar 19 14:19:57 2014 vincent leroy
##

SRC			= open_elf_file.c \
			  parse_elf_file.c \
			  function_name_in_elf.c \

NAME_SHARED	= libaelf.so
NAME_STATIC	= libaelf.a

CC			= gcc
LD			= ld
AR			= ar rc
RAN			= ranlib

OBJ_SHARED	= $(SRC:.c=.shared.o)
OBJ_STATIC	= $(SRC:.c=.static.o)

RM			= rm -f

MAKE		= make -C

CFLAGS		= -Wall -Wextra -Werror
CFLAGS		+= -O2

LDFLAGS		= -s

all: shared

shared:	CFLAGS += -fPIC
shared:	$(OBJ_SHARED)
	$(LD) -G $(OBJ_SHARED) $(LDFLAGS) -o $(NAME_SHARED)

static:	$(OBJ_STATIC)
	$(AR) $(NAME_STATIC) $(OBJ_STATIC)
	$(RAN) $(NAME_STATIC)

clean:
	$(RM) $(OBJ_SHARED) $(OBJ_STATIC) *.swp *~ *#

fclean:	clean
	$(RM) $(NAME_SHARED) $(NAME_STATIC)

re:	fclean all

%.shared.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.static.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:	all shared static clean fclean re
