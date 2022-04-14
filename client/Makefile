PWD = $(shell pwd)

NAME = client

CC = clang++

CFLAGS = -Wall -Wextra -Werror -g -std=c++98

all: $(NAME) 

$(NAME):
		@$(CC) -o $(NAME) client.cpp


./%.o: ./%.cpp
		@$(CC) -o $@ -c $<

clean:
	@rm -rf $(NAME)


fclean: clean
	@rm -rf $(NAME)

re: fclean all

test: re
	@./$(NAME)


.PHONY: all clean fclean re test