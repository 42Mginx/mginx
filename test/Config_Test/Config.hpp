#ifndef Config_HPP
#define Config_HPP

#include "ServerBlock.hpp"
#include "ConfigUtil.hpp"

class Config
{
private:
	ServerBlock _defaultConf;
	// server_name으로 ServerBlock 가져갈 수 있도록 함.
	std::vector<ServerBlock> _serverBlocks;
	std::vector<t_listen> _allListens;

public:
	Config();
	Config(std::string config_path);
	~Config();

	ServerBlock _initDefaultServer(const char *filename);
	int parseProcess(std::string config_path);
	fileVector readFile(std::string config_path_str);
	fileVector split(std::string str, std::string charset);
	std::vector<t_listen> HIHI();
	std::vector<t_listen> parseAllListens(std::vector<ServerBlock>::const_iterator serverBlock) const;
	void passMembers(std::vector<ServerBlock>::const_iterator server);
	// get 함수
	ServerBlock getDefaultConf();
	std::vector<ServerBlock> getServerBlock();
	std::vector<t_listen> getAllListens();
};

#endif