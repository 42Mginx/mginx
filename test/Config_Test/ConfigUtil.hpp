#ifndef CONFIG_UTIL
# define CONFIG_UTIL

#include "../Webserver_test/Webserver.hpp"
#include "ServerBlock.hpp"
#include <iostream>
#include <string>
#include <map>
#include <vector>

# define DEFAULT_CONFIG_PATH "./default.conf"
# define READER_BUFFER_SIZE 1024
typedef std::vector<std::string> fileVector;

bool isDigits(const std::string &str);
unsigned int	strToIp(std::string strIp);

#endif