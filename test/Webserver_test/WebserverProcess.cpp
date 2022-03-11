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
    return _connected_fd;  // success: fd, fail: -1
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

    _req += std::string(buffer);
    // find header index
    size_t header_index = _req.find("\r\n\r\n");
    std::cout << "header index: " << header_index << std::endl;
    // if header exist
    if (header_index != std::string::npos) {
        std::cout << "header found" << std::endl;

        bool chuncked = isChunked();
        // chuncked 인데 아직 덜 받았으면 return 1 => 아직 기달류
        // chuncked 인데 다 받았거나, chuncked가 아니면 0 => 프로세스 진행
        std::cout << "chuncked: " << chuncked << std::endl;
        if (!chuncked || (chuncked && isFinalChunked())) {
            std::cout << "this is not chunked, or it's final one"
                      << isFinalChunked() << std::endl;
            ret = 0;  // 진행해(proceed)
        } else {
            // chunked process
            std::cout << "this is chunked, and it's not final one" << std::endl;
            ret = 1;  // 아직 진행하지마 기달려(need to wait req)
        }
        std::string key = "Content-Length: ";
        int content_len_location = getKeyLocation(key);
        if (content_len_location != -1) {  // conetent length가 있으면
            size_t body_size = std::atoi(_req.substr(content_len_location + key.size(), 10).c_str());
            size_t total_size = body_size + header_index + 4;
            if (_req.size() >= total_size) {
                std::cout << "prepared to proceed" << std::endl;
                ret = 0;  // proceed
            } else {
                std::cout << "need to wait more req" << std::endl;
                ret = 1;  // wait
            }
        }
    }
    // ****** process *******
    // request, response 객체 생성
    // Request request(buffer);
    // Response response;
    // config 에서 맞는 server block 찾아서 넘기기(host, port맞는 걸로)
    // response.run(request, serverBlock);
    // _res = response.getResponse();
    // **********************
    _res = "response";  //getRes();  // 합치면서 바꿔야함

    if (ret == 0 && _res.empty()) {  // req를 다 받았는데도 res가 없으면 에러 처리
        std::cout << "response가 비어있습니다." << std::endl;
        _ready_to_response = false;
        ret = -1;
    }
    if (ret == 0) {
        _ready_to_response = true;
        std::cout << "===> ready to response(WebserverProcess)" << std::endl;
    }
    return ret;  // success: 양수, fail: -1
};

int WebserverProcess::writeResponse(void) {
    std::cout << "===> write" << std::endl;
    int ret = write(_connected_fd, _res.c_str(), 30);
    _connected_fd = -1;
    _ready_to_response = false;
    return ret;  // success: 양수, fail: -1
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

int WebserverProcess::getKeyLocation(std::string key) {
    int location = _req.find(key);
    if (location == std::string::npos)
        return -1;
    return location;
}

bool WebserverProcess::isChunked(void) {
    if (_req.find("Content-Length: ") != std::string::npos)
        return false;
    if (_req.find("Transfer-Encoding: chunked") == std::string::npos)
        return false;
    return true;
}

bool WebserverProcess::isFinalChunked(void) {
    int endOfFile = _req.find("0\r\n\r\n");
    if (endOfFile == std::string::npos)
        return false;
    int request_size = _req.size();
    if (request_size != endOfFile + 5)
        return false;
    std::string substr = _req.substr(request_size - 5, 5);
    if (substr != "0\r\n\r\n")
        return false;
    return true;
}

std::string WebserverProcess::getRes(void) {  // 임시 함수
    return "HTTP/1.1 200 OK\r\n\
Content-Length: 30\r\n\
Content-Location: /index.html\r\n\
Content-Type: text/html\r\n\
Date: Fri, 04 Mar 2022 09:28:09 GMT\r\n\
Last-Modified: Sun, 13 Feb 2022 09:27:39 GMT\r\n\
Server: Weebserv/1.0.0 (Unix)\r\n\
Transfer-Encoding: identity\r\n\
            This is the default index yo !";
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
