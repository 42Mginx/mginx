#include "Mginx.hpp"
#include "webserv.hpp"

#define DEFAULT_CONFIG "./srcs/config/default/default.conf"

Mginx::Mginx(void): _config(DEFAULT_CONFIG)
{
}

Mginx::Mginx(const Mginx & src)
{
	if (this != &src) {
		*this = src;
	}
}

Mginx::~Mginx(void)
{
}

Mginx & Mginx::operator=(const Mginx & src)
{
	_config = src._config;
	_servers = src._servers;
	_fd_set = src._fd_set;
	_fd_size = src._fd_size;
	_max_fd = src._max_fd;
	return (*this);
}

// 여기까지 canonical