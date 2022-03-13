#ifndef Config_HPP
# define Config_HPP

#include "ConfigUtil.hpp"

class Config {
	private:
		Config();
		Config(std::string config_path);
		virtual ~Config(void);

		ServerBlock _defaultConf;
		// server_name으로 ServerBlock 가져갈 수 있도록 함.
		std::map<std::string, ServerBlock> _serverBlock;
		std::vector<t_listen> _allListens;

	public:
		int			parseProcess(std::string config_path);
		fileVector	readFile(std::string config_path_str);
		fileVector	split(std::string str, std::string charset);

		void		getAllListens();
		void		getServerBlock();
		void		getDefaultConf();
};

#endif