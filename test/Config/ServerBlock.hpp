#ifndef SERVERBLOCK_HPP
# define SERVERBLOCK_HPP

#include "../Webserver.hpp"
#include "./Config.hpp"
#include "Location.hpp"

class ServerBlock {
	private:
		std::string	_server_name;
		std::string	_listen;
		std::string	_root;
		int			_auto_index;
		std::string	_allowed_methods;
		std::string	error_page;
		int			_client_body_buffer_size;
		// path, 해당 path location 클래스 
		std::map<std::string, Location> _location;
	public:
		ServerBlock();
		~ServerBlock();

		std::string	get_server_name();
		std::string	get_listen();
		std::string	get_root();
		int			get_auto_index();
		std::string	get_allowed_methods();
		std::string	get_error_page();
		int			get_client_body_buffer_size();

		int		parseServerBlock(std::string config_path);
};

#endif