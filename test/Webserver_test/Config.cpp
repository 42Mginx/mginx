
#include "Config.hpp"
Config::Config() {}
void Config::parseProcess(std::string config_path) {
    std::cout << "// Config => parse process //" << std::endl;
    ServerBlock a("localhost:8000", "8000");
    ServerBlock b("localhost:8001", "8001");
    _server_block.insert(std::make_pair("localhost:8000", a));
    _server_block.insert(std::make_pair("localhost:8001", b));
}

std::vector<t_listen> Config::getAllListens() {
    std::vector<t_listen> listens_v;

    t_listen listen_info;
    listen_info.port = 8000;
    listen_info.host = 0;

    t_listen listen_info2;
    listen_info2.port = 8001;
    listen_info2.host = 0;

    listens_v.push_back(listen_info);
    listens_v.push_back(listen_info2);

    return listens_v;
};

std::map<std::string, ServerBlock> Config::getServerBlock() {
    return _server_block;
}