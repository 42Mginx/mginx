#ifndef SERVERBLOCK_HPP
# define SERVERBLOCK_HPP

#include "../Webserver_test/Webserver.hpp"
#include "./Config.hpp"
#include "Location.hpp"

class ServerBlock {
	private:
		std::string	_server_name;
		std::string	_listen;
		std::string	_root;
		std::string	_allowed_methods;
		std::string	error_page;
		int			_auto_index;
		int			_client_body_buffer_size;
		// path, 해당 path location 클래스 
		std::map<std::string, std::string> _directives;
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

		int			parseServerBlock(unsigned int &index, fileVector &file);
};

#endif