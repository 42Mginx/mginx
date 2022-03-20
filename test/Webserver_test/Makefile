PWD = $(shell pwd)

NAME = webserv

CC = clang++

CFLAGS = -Wall -Wextra -Werror -g -std=c++98

CLASSES = -I config -I request -I response -I webserver -I utils

##############
### sources ##
##############

MAIN = mginx

HEADER = mginx.hpp

WEBSERVER = Webserver WebserverProcess

CONFIG = Config ServerBlock

REQUEST = Request RequestUtil

RESPONSE = Response

SRCS = $(addsuffix .cpp, $(MAIN)) \
	$(addprefix webserver/, $(addsuffix .cpp, $(WEBSERVER))) \
	$(addprefix config/, $(addsuffix .cpp, $(CONFIG))) \
	$(addprefix request/, $(addsuffix .cpp, $(REQUEST))) \
	$(addprefix response/, $(addsuffix .cpp, $(RESPONSE))) \


OBJ_DIR = obj

OBJ_SUBDIR = $(addprefix $(OBJ_DIR)/, config request response webserver)

OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))

all: $(NAME) 
		$(CC) -o client client.cpp


$(NAME): $(OBJ_DIR) $(OBJS)
		$(CC) -I. -o $(NAME) $(OBJS)

$(OBJ_DIR)/%.o: ./%.cpp
		@printf "\033[0;33mGenerating objects... %-38.38s\r" $@
		@$(CC) $(CLASSES) -o $@ -c $<

$(OBJ_DIR):
	echo "make dir"
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OBJ_SUBDIR)

clean:
	echo "clean"
	rm -rf $(OBJ_DIR) $(OBJ_SUBDIR)


fclean: clean
	rm -rf $(NAME)
	rm -rf client
	echo "fclean"

re: fclean all


test: re
	./webserv abc
	# @osascript -e 'tell application "Terminal" to do script "cd $(PWD) && clear && ./server"'


.PHONY: all clean fclean re test