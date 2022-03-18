#ifndef Config_HPP
# define Config_HPP

#include "ServerBlock.hpp"
#include "ConfigUtil.hpp"

class Config {
	private:
		ServerBlock						_defaultConf;
		// server_name으로 ServerBlock 가져갈 수 있도록 함.
		std::vector<ServerBlock>		_serverBlock;
		std::vector<t_listen>			_allListens;

	public:
		Config();
		Config(std::string config_path);
		~Config();

		int			parseProcess(std::string config_path);
		fileVector	readFile(std::string config_path_str);
		fileVector	split(std::string str, std::string charset);

		void		getAllListens();
		void		getServerBlock();
		void		getDefaultConf();
};

#endif