##
## Makefile
##
## Made by vincent leroy
## Login   <leroy_v@epitech.eu>
##
## Started on  Mon Jun 10 16:16:38 2013 vincent leroy
## Last update Sat Mar 01 15:12:53 2014 vincent leroy
##

SRC		= main.c

NAME	= create_syscall

CFLAGS	= -Wall -Wextra -W -Werror

LDFLAGS	=

OBJ		= $(SRC:.c=.o)

RM		= rm -f

CC		= gcc

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $(NAME)

clean:
	$(RM) $(OBJ) *.swp *~ *#
	$(RM) syscall.h

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
