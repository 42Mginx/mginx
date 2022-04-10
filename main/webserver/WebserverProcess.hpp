#ifndef WEBSERVERPROCESS_HPP
#define WEBSERVERPROCESS_HPP

#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Webserver.hpp"

#define RECV_SIZE 65536
#define RETURN_PROCEED 0
#define RETURN_WAIT 1
#define RETURN_ERROR -1

class WebserverProcess {
   private:
    int _socket_fd;
    int _connected_fd;
    t_listen _listen_info;
    struct sockaddr_in _addr;
    bool _ready_to_response;  // flag to indicate ready state
    int _sent;

    std::string _req;
    Request _request;
    std::string _res;
    Response _response;

    Config *_config;

    // sub
    int _prev_req_end_index;

    WebserverProcess(void);

   public:
    size_t _write_ret_sum;

    WebserverProcess(t_listen const &listen, Config &config);
    WebserverProcess(WebserverProcess const &src);
    ~WebserverProcess(void);
    WebserverProcess &operator=(WebserverProcess const &src);

    // action
    int setup(void);
    int accept(void);
    int readRequest(void);
    int process(void);
    int writeResponse(void);
    void clear(void);

    // util
    void setAddr(void);
    bool isChunked(void);
    bool isFinalChunked(void);
    int findKeyIndex(std::string key);
    ServerBlock findServerBlock();
    bool listenMatched(std::vector<t_listen> listens);
    void decodeChunk();

    // getter
    int getFd(void);
    int getConnectedFd(void);
    bool getReadyToResponse(void);
};

#endif
