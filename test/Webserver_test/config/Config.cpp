
#include "Config.hpp"

Config::Config() {
    // _defaultConf = parseProcess(DEFAULT_CONFIG_PATH);
}

Config::Config(std::string config_path) {
    // _defaultConf = parseProcess(DEFAULT_CONFIG_PATH);
    parseProcess(config_path);
}

void Config::parseProcess(std::string config_path) {
    std::vector<std::string> name_list;
    name_list.push_back("something");
    name_list.push_back("banana");
    name_list.push_back("localhost");

    std::vector<t_listen> listen1;
    t_listen listen;
    listen.port = 8000;
    listen.host = 0;
    listen1.push_back(listen);
    listen.port = 8001;
    listen.host = 0;
    listen1.push_back(listen);

    std::vector<t_listen> listen2;
    listen.port = 8002;
    listen.host = 0;
    listen2.push_back(listen);
    listen2.push_back(listen);

    ServerBlock a(name_list, listen1);
    ServerBlock b(name_list, listen2);
    _server_block.push_back(a);
    _server_block.push_back(b);
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

std::vector<ServerBlock> Config::getServerBlock() {
    return _server_block;
}

Config::~Config() {}