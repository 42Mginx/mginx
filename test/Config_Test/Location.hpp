#ifndef LOCATION_HPP
# define LOCATION_HPP

#include "../Webserver_test/Webserver.hpp"

class Location {
	private:
		std::string	_root;
		int			_auto_index;
		std::string _allowed_methods;
		std::string	_error_page;
		int			_client_body_buffer_size;
		std::string	_cgi_param;
		std::string	_cgi_pass;
		std::string _index;
		std::string _alias;
	public:
		Location();
		~Location();
		
		std::string get_root();
		int			get_auto_index();
		std::string	get_allowed_methods();
		std::string	get_error_page();
		int			get_client_body_buffer_size();
		std::string	get_cgi_param();
		std::string	get_cgi_pass();
		std::string get_index();
		std::string get_alias();

		void		parseLocationBlock(std::string location_path);
};

#endif