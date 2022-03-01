#ifndef Mginx_HPP
# define Mginx_HPP

# include <iostream>
# include <string>

class Mginx {
public:
	Mginx(void);
	Mginx(const Mginx & src);
	~Mginx(void);

	Mginx & operator=(const Mginx & src);

	void	ParsingConfig(std::string fileconf);
	int		setup(void);
	void	run(void);
	void	clean(void);
private:
	Config						_config;
	std::map<long, Server>		_servers;
	std::map<long, Server *>	_sockets;
	std::vector<int>			_ready;
	fd_set						_fd_set;
	unsigned int				_fd_size;
	long						_max_fd;
};

#endif