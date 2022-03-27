#ifndef SERVERBLOCK_HPP
#define SERVERBLOCK_HPP
#include <iostream>
#include <vector>

#include "listen.hpp"
class ServerBlock {
    std::vector<std::string> _server_name;
    std::vector<t_listen> _listen;

   public:
    ServerBlock(std::vector<std::string> server_name, std::vector<t_listen> listen);
    std::vector<std::string> getServerName() const;
    std::vector<t_listen> getListen() const;
};

#endif