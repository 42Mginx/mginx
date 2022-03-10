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
    std::cout << "===> read(WebserverProcess) " << ret << std::endl;

    if (ret <= 0) {
        return -1;
    }

    // std::cout << "buffer : " << buffer << std::endl;
    std::cout << "===> ready to response(WebserverProcess)" << std::endl;

    _request += std::string(buffer);
    // find header index
    size_t header_index = _request.find("\r\n\r\n");
    // if header exist
    if (header_index != std::string::npos) {
        std::cout << "header found" << std::endl;
        // if (hasContentLen()) {
        //     // 뭔가를 체크(길이 체크 하는 듯)
        //         return (1); // 아무것도 안함
        //     //
        //         return (0); // 진행해
        // }
        bool chuncked = isChunked();
        // chuncked 인데 아직 덜 받았으면 return 1 => 아직 기달류
        // chuncked 인데 다 받았거나, chuncked가 아니면 0 => 프로세스 진행
        if (!chuncked || (chuncked && isFinalChunked())) {
            std::cout << "this is not chunked, or it's final one"
                      << isFinalChunked() << std::endl;
            return 0; // 진행해
        } else {
            // chunked process
            std::cout << "this is chunked, and it's not final one" << std::endl;
            return 1; // 아직 진행하지마 기달려
        }
    }
    return 1; // 기달려?
    // request, response 객체 생성
    // Request request(buffer);
    // Response response;
    // config 에서 맞는 server block 찾아서 넘기기(host, port맞는 걸로)
    // response.run(request, serverBlock);
    // _response = response.getResponse();
    _response = "HTTP/1.1 200 OK\r\n\
Content-Length: 30\r\n\
Content-Location: /index.html\r\n\
Content-Type: text/html\r\n\
Date: Fri, 04 Mar 2022 09:28:09 GMT\r\n\
Last-Modified: Sun, 13 Feb 2022 09:27:39 GMT\r\n\
Server: Weebserv/1.0.0 (Unix)\r\n\
Transfer-Encoding: identity\r\n\
            This is the default index yo !";
    if (_response.empty()) {
        std::cout << "response가 비어있습니다." << std::endl;
        _ready_to_response = false;
        return -1;
    }
    _ready_to_response = true;
    return ret; // success: 양수, fail: -1
};

int WebserverProcess::writeResponse(void) {
    std::cout << "===> write" << std::endl;
    int ret = write(_connected_fd, _response.c_str(), 30);
    _connected_fd = -1;
    _ready_to_response = false;
    return ret; // success: 양수, fail: -1
};

void WebserverProcess::clear(void) {
    std::cout << "webserver process clear" << std::endl;
    // 해당 프로세스의 모든 열려있는 소켓 닫기
    if (_socket_fd > 0) {
        close(_socket_fd);
    }
    if (_connected_fd > 0) {
        close(_connected_fd);
    }
}

// util
void WebserverProcess::setAddr(void) {
    int addrlen = sizeof(_addr);
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = _listen_info.host || INADDR_ANY;
    std::cout << _listen_info.port << std::endl;
    _addr.sin_port = htons(_listen_info.port);
};

bool WebserverProcess::hasContentLen(void) {
    if (_request.find("Content-Length: ") == std::string::npos)
        return false;
    return true;
}

bool WebserverProcess::isChunked(void) {
    if (_request.find("Content-Length: ") != std::string::npos)
        return false;
    if (_request.find("Transfer-Encoding: chunked") == std::string::npos)
        return false;
    return true;
}

bool WebserverProcess::isFinalChunked(void) {
    int endOfFile = _request.find("0\r\n\r\n");
    if (endOfFile == std::string::npos)
        return false;
    int request_size = _request.size();
    if (request_size != endOfFile + 5)
        return false;
    std::string substr = _request.substr(request_size - 5, 5);
    if (substr != "0\r\n\r\n")
        return false;
    return true;
}

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
    _ready_to_response = src._ready_to_response;
    _addr = src._addr;
    return (*this);
}
