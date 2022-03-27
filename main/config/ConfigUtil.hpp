#ifndef CONFIG_UTIL
#define CONFIG_UTIL

#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <listen.hpp>
#include <map>
#include <set>
#include <string>
#include <unistd.h>
#include <vector>
#define DEFAULT_CONFIG_PATH "./test/conf/default.conf"
#define READER_BUFFER_SIZE 1024
typedef std::vector<std::string> fileVector;

// typedef struct s_listen {
//     unsigned int host;
//     int port;
// } t_listen;

bool isDigits(const std::string &str);
unsigned int strToIp(std::string strIp);
#include "ServerBlock.hpp"

#endif