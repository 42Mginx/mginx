#include "Request.hpp"


/*
std::string 								_method;
std::string									_target_path;

std::string									_query;
std::string									_version;
std::map<std::string, std::string>			_headers;
std::string									_body;
int											_status_code;
std::vector<std::string>					_valid_methods;
*/

Request::Request()
{
	_method ="GET";
	_target_path ="files/default_error_pages";
	// _target_path ="";

	_query = "a=42";
	_version = "1.1";
	_body="Hello World!";
	_status_code = 200;
}

Request::~Request()
{
}
void	Request::coutRequest()
{
	std::cout<<"test"<<std::endl;
}



//getter
const std::string&	Request::getMethod() const
{
	return this->_method;
}

const std::string&	Request::getTargetPath() const
{
	return this->_target_path;
}

const std::string&	Request::getQuery() const
{
	return this->_query;
}

const std::string&	Request::getVersion() const
{
	return this->_version;
}

const std::map<std::string, std::string>&	Request::getHeaders() const
{
	return this->_headers;
}

const std::string&	Request::getBody() const
{
	return this->_body;
}

const int	Request::getStatus() const
{
	return this->_status_code;
}
