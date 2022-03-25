#ifndef CONFIG_UTIL
#define CONFIG_UTIL

#include <fcntl.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "listen.hpp"

#define DEFAULT_CONFIG_PATH "./test/conf/default.conf"
#define READER_BUFFER_SIZE 1024

typedef std::vector<std::string> fileVector;

bool isDigits(const std::string &str);
unsigned int strToIp(std::string strIp);

#include "ServerBlock.hpp"
#endif