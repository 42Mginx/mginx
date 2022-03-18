#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP
// CPP Includes
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

// CPP Containers
#include <algorithm>
#include <iterator>
#include <list>
#include <map>
#include <set>
#include <utility>
#include <vector>

// C Includes
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// C System
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

// C Network
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>

#include "Config.hpp"
#define BUF_SIZE 4096
#include "listen.hpp"

// color
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"

class WebserverProcess;

class Webserver {
   private:
    // tmp
    // std::vector<t_listen> getListens();

    // 멤버변수
    long _max_fd;
    fd_set _fd_set;
    std::vector<int> _socket_fd_v;
    std::vector<int> _connected_fd_v;
    std::vector<WebserverProcess> _process_v;
    std::vector<t_listen> _listens_v;
    fd_set _reading_set;
    fd_set _writing_set;
    Config _config;

    // private method
    void init();

   public:
    // occf
    Webserver(void);
    Webserver(const Webserver &src);
    ~Webserver(void);
    Webserver &operator=(const Webserver &src);

    // action
    void parseConfig(std::string config_path);
    int setup();
    int run();
    void handle_error(std::string const &error_message);
};

#endif