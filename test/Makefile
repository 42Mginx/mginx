PWD = $(shell pwd)


all: client.cpp server.cpp
	clang++ -o client client.cpp
	clang++ -o server server.cpp

clean:
	rm -rf client
	rm -rf server


fclean: clean
	echo "fclean"

re: fclean all

test:
	# ./server
	# ./client
	@osascript -e 'tell application "Terminal" to do script "cd $(PWD) && clear && ./server"'


.PHONY: all clean fclean re test




# NAME = webserv

# CC = clang++

# CFLAGS = -Wall -Wextra -Werror -g -std=c++98# -fsanitize=address

# SRC = client.cpp\
# 		server.cpp


# SRC_DIR =


# OBJ_DIR = objs

# OBJ_BUILD = $(addprefix $(OBJ_DIR)/, $(SRC:cpp=o))

# all: $(NAME)

# $(NAME): $(OBJ_DIR) $(OBJ_BUILD)
# 	@echo "\n"
# 	@echo "\033[0;32mCompiling webserv..."
# 	@$(CC) $(CFLAGS) -o $(NAME) $(OBJ_BUILD)
# 	@echo "\n\033[0mDone !"

# $(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp 
# 	@printf "\033[0;33mGenerating webserv objects... %-38.38s\r" $@
# 	@$(CC) $(CFLAGS) -o $@ -c $<

# $(OBJ_DIR):
# 	@mkdir -p $(OBJ_DIR)



# clean:
# 	@echo "\n\033[0;31mDeleting objects..."
# 	@rm -rf $(OBJ_DIR)
# 	@echo "\033[0m"

# fclean: clean
# 	@echo "\033[0;31mDeleting executable..."
# 	@rm -f $(NAME)

# re: fclean all





# test: all
# 	@asascript -e 'tell application "Termainal" to do script "cd $(PWD) && clear && ./'