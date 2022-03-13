#ifndef SERVERBLOCK_HPP
#define SERVERBLOCK_HPP
#include <iostream>

class ServerBlock {
    std::string _server_name;
    std::string _listen;

   public:
    ServerBlock(std::string server_name, std::string listen);
    std::string getServerName();
    std::string getListen();
};

#endif