#include "Config.hpp"

Config::Config() { ServerBlock::_initDefaultServer(DEFAULT_CONFIG_PATH); }

Config::Config(std::string config_path) {
    ServerBlock::_initDefaultServer(DEFAULT_CONFIG_PATH);
    parseProcess(config_path);
    parseListenAndFillBlank();
}

Config::~Config() {}

void Config::parseProcess(std::string config_path) {
    fileVector file;
    unsigned int fileSize;

    file = readFile(config_path);
    fileSize = file.size();
    for (unsigned int i = 0; i < fileSize; i++) {
        if (file[i] == "server") {
            ServerBlock tmpServerBlock;
            ++i;
            if (file[i] != "{") {
                std::cerr << "Error: expected '{' after server directive."
                          << std::endl;
                throw ConfigException();
            }
            ++i;
            if (!tmpServerBlock.parseServerBlock(i, file)) {
                std::cerr << "Error: error in config file [" << config_path
                          << "]" << std::endl;
                throw ConfigException();
            }
            this->_serverBlocks.push_back(tmpServerBlock);
        } else {
            std::cerr << "Error: unknown directive [" << file[i] << "]"
                      << std::endl;
            throw ConfigException();
        }
    }
    parseListenAndFillBlank();
}

void Config::parseAllListens(
    std::vector<t_listen> &allListens,
    std::vector<ServerBlock>::const_iterator serverBlock) {
    // listen 백터를 순환
    std::vector<t_listen> listenVec = serverBlock->getListen();
    for (std::vector<t_listen>::iterator listen = listenVec.begin();
         listen != listenVec.end(); listen++) {
        std::vector<t_listen>::iterator i = allListens.begin();
        for (; i != allListens.end(); i++)
            if (listen->host == i->host && listen->port == i->port)
                break;
        if (i == allListens.end()) {
            allListens.push_back(*listen);
        }
    }
}

void Config::parseListenAndFillBlank() {
    std::vector<t_listen> allListens;
    for (std::vector<ServerBlock>::iterator serverBlock = _serverBlocks.begin();
         serverBlock != _serverBlocks.end(); serverBlock++) {
        parseAllListens(allListens, serverBlock);
    }
    _allListens = allListens;
}

std::vector<ServerBlock> Config::getServerBlock() {
    return this->_serverBlocks;
}

std::vector<t_listen> Config::getAllListens() { return this->_allListens; }

const char *Config::FileNotFoundException::what() const throw() {
    return "Exception thrown: could not open configuration file";
}

const char *Config::ConfigException::what() const throw() {
    return "Exception thrown: Confile Parsing Error";
}