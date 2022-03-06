#include "WebserverProcess.hpp"

// action
int WebserverProcess::setup(void) {
    _socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket_fd != -1) {
        fcntl(_socket_fd, F_SETFL, O_NONBLOCK);
        ::bind(_socket_fd, (struct sockaddr *)&_addr, sizeof(_addr));
        ::listen(_socket_fd, 1000);
    }
    return _socket_fd;
};

int WebserverProcess::accept(void) {
    _connected_fd = ::accept(_socket_fd, NULL, NULL);
    std::cout << "===> accept(WebserverProcess) " << _connected_fd << std::endl;
    return _connected_fd; // success: fd, fail: -1
};

int WebserverProcess::readRequest(void) {
    char buffer[BUF_SIZE] = {
        0,
    };
    int ret = read(_connected_fd, buffer, BUF_SIZE - 1);
    std::cout << "===> read(WebserverProcess)" << std::endl;
    if (ret > 0) {

        std::cout << buffer << std::endl;
    }
    return ret; // success: 양수, fail: -1
};

int WebserverProcess::readyToResponse(void) {
    std::cout << "===> ready to response(WebserverProcess)" << std::endl;
    Request request;
    // process()
    _ready_to_response = true;
    return 0; // fail: -1;
}
int WebserverProcess::writeResponse(void) {
    std::cout << "===> write" << std::endl;
    // Response response;
    // std::string res = response.getResponse();
    std::string res = "res";
    int ret = write(_connected_fd, res.c_str(), 30);
    _connected_fd = -1;
    _ready_to_response = false;
    return ret; // success: 양수, fail: -1
};

// util
void WebserverProcess::setAddr(void) {
    int addrlen = sizeof(_addr);
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = _listen_info.host || INADDR_ANY;
    std::cout << _listen_info.port << std::endl;
    _addr.sin_port = htons(_listen_info.port);
};

// setter

// getter
int WebserverProcess::getFd(void) { return _socket_fd; };
int WebserverProcess::getConnectedFd(void) { return _connected_fd; };
bool WebserverProcess::getReadyToResponse(void) { return _ready_to_response; };

// occf
WebserverProcess::WebserverProcess(void) {}
WebserverProcess::WebserverProcess(t_listen const &listen) {
    _listen_info = listen;
    _socket_fd = -1;
    _connected_fd = -1;
    _ready_to_response = false;
    this->setAddr();
}
WebserverProcess::WebserverProcess(WebserverProcess const &src) { *this = src; }
WebserverProcess::~WebserverProcess(void) {}
WebserverProcess &WebserverProcess::operator=(WebserverProcess const &src) {
    _listen_info = src._listen_info;
    _socket_fd = src._socket_fd;
    _connected_fd = src._connected_fd;
    _addr = src._addr;
    return (*this);
}
