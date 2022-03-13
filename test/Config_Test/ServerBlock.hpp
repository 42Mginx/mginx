#ifndef SERVERBLOCK_HPP
# define SERVERBLOCK_HPP

#include "../Webserver_test/Webserver.hpp"
#include "./Config.hpp"
#include "Location.hpp"

class ServerBlock {
	private:
		std::vector<std::string>	_server_name; // 여러개일 수 있음
		std::vector<t_listen>		_listen; // 여러개일 수 있음
		std::string					_root; // 얘는 한개
		std::set<std::string>		_allowed_methods; // 얘는 여러개
		std::vector<std::string>	_error_page; // 얘도 여러개
		std::string					_cgi_pass; // string
		int							_client_body_buffer_size; // 한 개
		bool						_auto_index; // 한 개

		std::map<std::string, Location> _location; // path, 해당 path location 클래스 
		std::map<std::string, void (ServerBlock::*)(std::vector<std::string>)> directivesParseFunc;

	public:
		ServerBlock();
		~ServerBlock();
		ServerBlock						&operator=(ServerBlock const &src);
		class	ExceptionInvalidArguments: public std::exception {
			virtual const char	*what() const throw();
		};

		std::string	get_server_name();
		std::string	get_listen();
		std::string	get_root();
		int			get_auto_index();
		std::string	get_allowed_methods();
		std::string	get_error_page();
		int			get_client_body_buffer_size();

		// 지시어(directives) 모음 맵
		void		_initDirectivesMap();

		// 지시어 파싱 함수
		void		addServerName(std::vector<std::string> args);
		void		addListen(std::vector<std::string> args);
		void		addRoot(std::vector<std::string> args);
		void		addAllowedMethods(std::vector<std::string> args);
		void		addErrorPage(std::vector<std::string> args);
		void		addCgiPass(std::vector<std::string> args);
		void		addClientBodyBufferSize(std::vector<std::string> args);
		void		addAutoIndex(std::vector<std::string> args);

		int			parseServerBlock(unsigned int &index, fileVector &file);
};

#endif