#ifndef CONFIG_UTIL
# define CONFIG_UTIL

#include <iostream>
#include <unistd.h>
#include <fstream>
#include <fcntl.h>
#include <string>
#include <map>
#include <vector>
#include <set>

# define DEFAULT_CONFIG_PATH "./default.conf"
# define READER_BUFFER_SIZE 1024
typedef std::vector<std::string> fileVector;

typedef struct s_listen {
    unsigned int host;
    int port;
} t_listen;

bool isDigits(const std::string &str);
unsigned int	strToIp(std::string strIp);
#include "ServerBlock.hpp"


#endif