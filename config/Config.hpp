#ifndef Config_HPP
#define Config_HPP

#include "ConfigUtil.hpp"
#include "ServerBlock.hpp"

class Config {
  private:
    ServerBlock _defaultConf;
    std::vector<ServerBlock> _serverBlocks;
    std::vector<t_listen> _allListens;

  public:
    Config();
    Config(std::string config_path);
    ~Config();

    void parseProcess(std::string config_path);
    void parseListenAndFillBlank();
    void parseAllListens(std::vector<t_listen> &allListens,
                         std::vector<ServerBlock>::const_iterator serverBlock);
    // get 함수
    std::vector<ServerBlock> getServerBlock();
    std::vector<t_listen> getAllListens();

    // throw err
    class FileNotFoundException : public std::exception {
        virtual const char *what() const throw();
    };
    class ConfigException : public std::exception {
        virtual const char *what() const throw();
    };
};

#endif