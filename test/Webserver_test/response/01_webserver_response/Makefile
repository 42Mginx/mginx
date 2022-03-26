PWD = $(shell pwd)

all: mginx.cpp Webserver.cpp response/Response.cpp response/ResponseHeader.cpp  autoindex/AutoindexGenerator.cpp request/Request.cpp response/GetConf.cpp ServerBlock.cpp cgi/CgiHandler.cpp
	clang++ -o server_file Webserver.cpp mginx.cpp response/Response.cpp response/ResponseHeader.cpp  autoindex/AutoindexGenerator.cpp request/Request.cpp response/GetConf.cpp ServerBlock.cpp cgi/CgiHandler.cpp

clean:
	rm -rf server_file


fclean: clean
	echo "fclean"

re: fclean all

test:
	# ./server_file
	# ./client
	@osascript -e 'tell application "Terminal" to do script "cd $(PWD) && clear && ./server_file"'


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
