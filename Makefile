PWD = $(shell pwd)

NAME = webserv

CC = clang++

CXXFLAGS = -Wall -Wextra -Werror -g3 -std=c++98

CLASSES = -I config -I request -I response -I webserver -I utils -I cgi -I autoindex

##############
### sources ##
##############

MAIN = mginx

HEADER = mginx.hpp

WEBSERVER = Webserver WebserverProcess

CONFIG = Config ConfigUtil ServerBlock
# CONFIG = Config ServerBlock

REQUEST = Request RequestUtil

RESPONSE = Response ResponseHeader GetConf

AUTOINDEX = AutoIndexGenerator

CGI = CgiHandler

SRCS = $(addsuffix .cpp, $(MAIN)) \
	$(addprefix webserver/, $(addsuffix .cpp, $(WEBSERVER))) \
	$(addprefix config/, $(addsuffix .cpp, $(CONFIG))) \
	$(addprefix request/, $(addsuffix .cpp, $(REQUEST))) \
	$(addprefix response/, $(addsuffix .cpp, $(RESPONSE))) \
	$(addprefix autoindex/, $(addsuffix .cpp, $(AUTOINDEX))) \
	$(addprefix cgi/, $(addsuffix .cpp, $(CGI))) \


OBJ_DIR = obj

OBJ_SUBDIR = $(addprefix $(OBJ_DIR)/, config request response webserver cgi autoindex)

OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))

CLIENT_DIR = client

LOG_FILE = response.txt

TESTER_DIR = $(PWD)/test/42_tester

all: $(NAME)

$(NAME): $(CLIENT) $(OBJ_DIR) $(OBJS)
		@$(MAKE) -C $(CLIENT_DIR) re
		@$(CC) -g3 -fsanitize=address -I. -o $(NAME) $(OBJS)

$(OBJ_DIR)/%.o: ./%.cpp
		@printf "\033[0;33mGenerating objects... %-38.38s\r" $@
		@$(CC) -g3 -fsanitize=address $(CLASSES) -o $@ -c $<

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OBJ_SUBDIR)


clean:
	@rm -rf $(OBJ_DIR) $(OBJ_SUBDIR)
	@$(MAKE) -C $(CLIENT_DIR) clean


fclean: clean
	@rm -rf $(NAME)
	@rm -rf $(LOG_FILE)
	@$(MAKE) -C $(CLIENT_DIR) fclean

re: fclean all

test: re
	@osascript -e 'tell application "Terminal" to do script "cd $(PWD) && clear && ./client/client"'
	@osascript -e 'tell application "Terminal" to activate'
	@./webserv ./test/conf/mac_test.conf

tester: re
	@rm -rf $(LOG_FILE)
	@touch $(LOG_FILE)
	@osascript -e 'tell application "Terminal" to do script "cd $(TESTER_DIR) && clear "'
	@osascript -e 'tell application "Terminal" to activate'
	@osascript -e 'tell application "System Events" to keystroke "./tester http://localhost:8000" '
	@./webserv ./test/conf/mac_test.conf

run: re
	@./webserv ./test/conf/mac_test.conf

.PHONY: all clean fclean re test run tester
