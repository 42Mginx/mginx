#ifndef Config_HPP
# define Config_HPP

# define DEFAULT_CONFIG_PATH "./default.conf"

#include "ServerBlock.hpp"
#include "../Webserver.hpp"
#include <iostream>
#include <string>
#include <map>
#include <vector>

# define READER_BUFFER_SIZE 1024
typedef std::vector<std::string> fileVector;

class Config {
	private:
		Config();
		Config(std::string config_path);
		virtual ~Config(void);

		ServerBlock _default_conf;
		// server_name으로 ServerBlock 가져갈 수 있도록 함.
		std::map<std::string, ServerBlock> _server_block;
		std::vector<t_listen> _all_listens;

	public:
		int			parseProcess(std::string config_path);
		fileVector	readFile(std::string config_path_str);
		fileVector	split(std::string str, std::string charset);

		void		getAllListens();
		void		getServerBlock();
		void		getDefaultConf();
};

#endif