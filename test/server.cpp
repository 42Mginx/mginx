#include "Server.hpp"

// Overloaders

Server &	Server::operator=(const Server & src)
{
	_listen = src._listen;
	_fd = src._fd;
	_addr = src._addr;
	return (*this);
}

// Constructors and destructors

Server::Server(void)
{
	// Wait, that's illegal !
}

Server::Server(const t_listen & listen)
{
	_listen = listen;
	_fd = -1;
	this->setAddr();
}

Server::Server(const Server & src)
{
	*this = src;
}

Server::~Server(void)
{
}
