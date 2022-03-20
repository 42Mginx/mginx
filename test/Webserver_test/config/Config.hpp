#ifndef CONFIG_HPP
#define CONFIG_HPP
#include <iostream>
#include <map>
#include <vector>

#include "ServerBlock.hpp"
#include "listen.hpp"

class Config {
    std::map<std::string, ServerBlock> _server_block;

   public:
    Config();

    void parseProcess(std::string config_path);
    std::vector<t_listen> getAllListens();
    std::map<std::string, ServerBlock> getServerBlock();
};

#endif