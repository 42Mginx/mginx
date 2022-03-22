#ifndef CONFIG_HPP
#define CONFIG_HPP
#include <iostream>
#include <map>
#include <vector>

#include "ServerBlock.hpp"
#include "listen.hpp"

class Config {
    std::vector<ServerBlock> _server_block;

   public:
    Config();
    Config(std::string config_path);
    ~Config();

    void parseProcess(std::string config_path);
    std::vector<t_listen> getAllListens();
    std::vector<ServerBlock> getServerBlock();
};

#endif