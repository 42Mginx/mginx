#include "ServerBlock.hpp"


ServerBlock::ServerBlock(void) {

	// _server_name = ;	// 여러개일 수 있음
	// std::vector<t_listen> _listen;			// 여러개일 수 있음
	_root = "";						// 얘는 한개
	std::set<std::string> tempMethod;
	// tempMethod.insert("GET");
	tempMethod.insert("POST");
	tempMethod.insert("PUT");
	_allowed_methods = tempMethod; // 얘는 여러개

	std::map<int, std::string> tempError;
	tempError[400] = "files/default_error_pages/400.html";
	tempError[403] = "files/default_error_pages/403.html";
	tempError[404] = "files/default_error_pages/404.html";
	tempError[405] = "files/default_error_pages/405.html";
	tempError[410] = "files/default_error_pages/410.html";
	tempError[413] = "files/default_error_pages/413.html";
	tempError[500] = "files/default_error_pages/500.html";


	_error_page = tempError; // 얘도 여러개
	_client_body_buffer_size = 1;			// 한 개
	_auto_index = false;						// 한 개

	// location directives
	// std::map<std::string, std::string> _cgi_param;
	_cgi_pass = "selfCgiTester";

		std::vector<std::string> tempIndex;
	tempIndex.push_back("index.html");
	_index = tempIndex; //파일명임
	// std::string _alias;
	// bool _aliasSet;


	//토요일날 하자
	// std::map<std::string, ServerBlock> _location; // path, 해당 path location 클래스
}
ServerBlock::~ServerBlock(void) {}

//getter

// get 함수들
std::vector<std::string> ServerBlock::getServerName() const
{
	return this->_server_name;
}

// std::vector<t_listen> ServerBlock::getListen() const
// {
// 	return this->_listen;
// }

std::string ServerBlock::getRoot() const
{
	return this->_root;
}

std::set<std::string> ServerBlock::getAllowedMethods() const
{
	return this->_allowed_methods;
}

std::map<int, std::string> ServerBlock::getErrorPage() const
{
	return this->_error_page;
}

int ServerBlock::getClientBodyBufferSize() const
{
	return this->_client_body_buffer_size;
}

bool ServerBlock::getAutoIndex() const
{
	return this->_auto_index;
}

std::map<std::string, std::string> ServerBlock::getCgiParam() const
{
	return this->_cgi_param;
}

std::string ServerBlock::getCgiPass() const
{
	return this->_cgi_pass;
}

std::map<std::string, ServerBlock> ServerBlock::getLocation() const
{
	return this->_location;
}

std::vector<std::string> ServerBlock::getIndex() const
{
	return this->_index;
}

std::string ServerBlock::getAlias() const
{
	return this->_alias;
}

bool ServerBlock::getAliasSet() const
{
	return this->_aliasSet;
}
