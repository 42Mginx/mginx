
#include "ServerBlock.hpp"

ServerBlock::ServerBlock(std::vector<std::string> server_name, std::string listen) {
    _server_name = server_name;
    _listen = listen;
};
std::vector<std::string> ServerBlock::getServerName() {
    return _server_name;
};
std::string ServerBlock::getListen() {
    return _listen;
};
