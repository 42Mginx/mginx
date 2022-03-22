
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
    name_list.push_back("a");
    name_list.push_back("b");
    name_list.push_back("c");
    ServerBlock a(name_list, "8000");
    ServerBlock b(name_list, "8001");
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