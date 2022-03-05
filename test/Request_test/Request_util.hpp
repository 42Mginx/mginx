#ifndef REQUEST_UTIL
# define REQUEST_UTIL

# include <iostream>
# include <map>
# include <string>
# include <vector>

std::string		readLine(const std::string &str, size_t& i);
std::string&	pop(std::string& str);
std::string		readKey(char *line);
std::string&	capitalize(std::string& str);
std::string&	capitalize(std::string& str);
std::string		readValue(char *line);

#endif