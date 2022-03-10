#ifndef WEBSERVERPROCESS_HPP
#define WEBSERVERPROCESS_HPP

#include "Request.hpp"
#include "Webserver.hpp"

class WebserverProcess {
  private:
    int _socket_fd;
    int _connected_fd;
    t_listen _listen_info;
    struct sockaddr_in _addr;
    bool _ready_to_response;

    std::string _request;
    std::string _response;

    WebserverProcess(void);
    bool isChunked(void);
    bool isFinalChunked(void);
    bool hasContentLen(void);

  public:
    WebserverProcess(t_listen const &listen);
    WebserverProcess(WebserverProcess const &src);
    ~WebserverProcess(void);
    WebserverProcess &operator=(WebserverProcess const &src);

    // action
    int setup(void);
    int accept(void);
    int readRequest(void);
    int writeResponse(void);
    void clear(void);

    // util
    void setAddr(void);

    // setter

    // getter
    int getFd(void);
    int getConnectedFd(void);
    bool getReadyToResponse(void);
};

#endif