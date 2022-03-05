#ifndef SERVER_HPP
# define SERVER_HPP

#include "Mginx.hpp"



class Server {

private:
    t_listen            _listen_info;
    int                 _socket_fd;
    int                 _connected_fd;  
    struct sockaddr_in  _socket_addr;

public:
 
    // occf
	Server(void);
	Server(const t_listen & listen);
	Server(const Server & src);
	~Server(void);

	Server & operator=(const Server & src);


};

#endif