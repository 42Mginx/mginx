#ifndef WEBSERVERPROCESS_HPP
#define WEBSERVERPROCESS_HPP

#include "Webserver.hpp"

class WebserverProcess {
  private:
    int _socket_fd;
    int _connected_fd;
    t_listen _listen_info;
    struct sockaddr_in _addr;
    bool _ready_to_response;

    WebserverProcess(void);

  public:
    WebserverProcess(t_listen const &listen);
    WebserverProcess(WebserverProcess const &src);
    ~WebserverProcess(void);
    WebserverProcess &operator=(WebserverProcess const &src);

    // action
    int setup(void);
    int accept(void);
    int readRequest(void);
    int readyToResponse(void);
    int writeResponse(void);

    // util
    void setAddr(void);

    // setter

    // getter
    int getFd(void);
    int getConnectedFd(void);
    bool getReadyToResponse(void);
};

#endif