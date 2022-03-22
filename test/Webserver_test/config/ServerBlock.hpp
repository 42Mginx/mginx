#ifndef SERVERBLOCK_HPP
#define SERVERBLOCK_HPP
#include <iostream>
#include <vector>

class ServerBlock {
    std::vector<std::string> _server_name;
    std::string _listen;

   public:
    ServerBlock(std::vector<std::string> server_name, std::string listen);
    std::vector<std::string> getServerName();
    std::string getListen();
};

#endif