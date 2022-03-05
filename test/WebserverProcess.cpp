#include "WebserverProcess.hpp"

// action
void WebserverProcess::setup(void) {

    // 생성
    _socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    // 구조체 bind
    ::bind(_socket_fd, (struct sockaddr *)&_addr, sizeof(_addr));
    // listen
    ::listen(_socket_fd, 1000);
};

int WebserverProcess::accept(void) {
    _connected_fd = ::accept(_socket_fd, NULL, NULL);
    std::cout << "===> accept" << _connected_fd << std::endl;
    return _connected_fd; // success: fd, fail: -1
};

int WebserverProcess::readRequest(void) {
    char buffer[BUF_SIZE] = {
        0,
    };
    int ret = read(_connected_fd, buffer, BUF_SIZE - 1);
    std::cout << "===> read" << std::endl;

    return ret; // success: 양수, fail: -1
};

int WebserverProcess::readyToResponse(void) {
    _ready_to_response = true;
    return 0; // fail: -1;
}
int WebserverProcess::writeResponse(void) {
    int ret = write(_connected_fd, "hello world form server", 30);
    std::cout << "===> write" << std::endl;
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

// Constructors and destructors
WebserverProcess::WebserverProcess(void) {
    // wait, that's illegal !
}

WebserverProcess::WebserverProcess(t_listen const &listen) {
    _listen_info = listen;
    _socket_fd = -1;
    _connected_fd = -1;
    _ready_to_response = false;
    this->setAddr();
}

WebserverProcess::WebserverProcess(WebserverProcess const &src) { *this = src; }

WebserverProcess::~WebserverProcess(void) {}

// overloaders

WebserverProcess &WebserverProcess::operator=(WebserverProcess const &src) {
    _listen_info = src._listen_info;
    _socket_fd = src._socket_fd;
    _connected_fd = src._connected_fd;
    _addr = src._addr;
    return (*this);
}
