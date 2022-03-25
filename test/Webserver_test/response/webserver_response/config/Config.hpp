#ifndef Config_HPP
#define Config_HPP

#include "ServerBlock.hpp"
#include "ConfigUtil.hpp"

class Config
{
	private:
		ServerBlock _defaultConf;
		std::vector<ServerBlock> _serverBlocks;
		std::vector<t_listen> _allListens;

	public:
		Config();
		Config(std::string config_path);
		~Config();

		void _initDefaultServer(const char *filename);
		void parseProcess(std::string config_path);

		fileVector readFile(std::string config_path_str);
		fileVector split(std::string str, std::string charset);

		void parseListenAndFillBlank();
		void parseAllListens(std::vector<t_listen> &allListens, std::vector<ServerBlock>::const_iterator serverBlock);
		void passMembers(ServerBlock &serverBlock);
		// get 함수
		ServerBlock getDefaultConf();
		std::vector<ServerBlock> getServerBlock();
		std::vector<t_listen> getAllListens();

		// throw err
		class FileNotFoundException : public std::exception
		{
			virtual const char *what() const throw();
		};
		class ConfigException : public std::exception
		{
			virtual const char *what() const throw();
		};
		
};

#endif