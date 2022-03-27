
#include "ServerBlock.hpp"

ServerBlock::ServerBlock(std::vector<std::string> server_name, std::vector<t_listen> listen) {
    _server_name = server_name;
    _listen = listen;
};
std::vector<std::string> ServerBlock::getServerName() const {
    std::cout << "get server name" << std::endl;
    return _server_name;
};
std::vector<t_listen> ServerBlock::getListen() const {
    std::cout << "get listens" << std::endl;
    return _listen;
};
