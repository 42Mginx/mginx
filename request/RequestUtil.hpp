#ifndef REQUEST_UTIL
#define REQUEST_UTIL

#include <iostream>
#include <map>
#include <string>
#include <vector>

std::string readLine(const std::string &str, size_t &i);
std::string &pop(std::string &str);
std::string readKey(const std::string &line);
std::string &capitalize(std::string &str);
std::string readValue(const std::string &line);
std::string &strip(std::string &str, char c);
std::string &to_upper(std::string &str);
std::string &to_lower(std::string &str);

#endif