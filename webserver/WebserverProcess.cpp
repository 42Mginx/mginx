#include "WebserverProcess.hpp"

#include <time.h>

// 소켓 fd 생성부터 listen까지
int WebserverProcess::setup(void) {
    _socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket_fd != -1) {
        if (bind(_socket_fd, (struct sockaddr *)&_addr, sizeof(_addr)) == -1) {
            return -1;
        }
        listen(_socket_fd, 1000);
    }
    return _socket_fd;
};

int WebserverProcess::accept(void) {
    _connected_fd = ::accept(_socket_fd, NULL, NULL);
    std::cout << "===> accept(WebserverProcess) " << _socket_fd << ":"
              << _connected_fd << std::endl;
    if (_connected_fd != -1) {
        fcntl(_connected_fd, F_SETFL, O_NONBLOCK);
    }
    return _connected_fd; // success: fd, fail: -1
};

int WebserverProcess::readRequest(void) {
    char buffer[BUF_SIZE] = {
        0,
    };
    int ret = recv(_connected_fd, buffer, BUF_SIZE - 1, 0);
    if (ret <= 0) {
        if (ret == 0) {
            std::cout << "\rConnection was closed by client.\n" << std::endl;
            return RETURN_CLOSE;
        } else if (ret == -1) {

            std::cout << "\rRead error, closing connection.\n" << std::endl;
        } else {
            std::cout << "read error!" << std::endl;
        }
        return -1;
    }
    _req += std::string(buffer);

    if (_header_index == 0 || _header_index == std::string::npos) {
        _header_index = _req.find("\r\n\r\n"); //*수정
    }
    _prev_req_end_index += ret - 1;
    std::cout << "read now: " << _prev_req_end_index << std::endl;
    if (_header_index != std::string::npos) {
        if (_req.find("Content-Length: ") == std::string::npos) {
            if (_req.find("Transfer-Encoding: chunked") != std::string::npos) {
                // chuncked 인데 다 받았거나, chuncked가 아니면 0 => PROCEED
                if (isFinalChunked()) {
                    ret = RETURN_PROCEED;
                } else
                    return (RETURN_WAIT);
            } else {
                ret = RETURN_PROCEED;
            }
        }

        std::string key = "Content-Length: ";
        int content_len_index = findKeyIndex(key);
        if (content_len_index != -1) {
            size_t body_size = std::atoi(
                _req.substr(content_len_index + key.size(), 10).c_str());
            size_t total_size = body_size + _header_index + 4;
            if (_req.size() >= total_size) {
                std::cout << "prepared to proceed" << std::endl;
                ret = RETURN_PROCEED;
            } else {
                std::cout << "need to wait more req" << std::endl;
                return (RETURN_WAIT);
            }
        }
    } else {
        return (RETURN_WAIT);
    }

    if (ret == RETURN_PROCEED)
    {
        std::cout << "\n--- request ---" << std::endl;
        std::cout << _req.substr(0, 250) << std::endl;
        std::cout << "---------------" << std::endl;
        ret = process();
    }
    if (ret == RETURN_ERROR) {
        std::cerr << "// empty response error //" << std::endl;
        _ready_to_response = false;
        return RETURN_ERROR;
    } else if (ret == RETURN_PROCEED) {
        std::cout << "// res is ready... //" << std::endl;
        _ready_to_response = true;
    }
    return ret; // success: 1(RETURN_WAIT)/0(RETURN_PROCEED)
};

int WebserverProcess::process(void) {
    // 0. chuncked 뒤에 chundked가 온 경우
    int chunked_index = _req.find("Transfer-Encoding: chunked");
    if (chunked_index != std::string::npos &&
        chunked_index < _req.find("\r\n\r\n")) {
        decodeChunk();
    }

    // 1. 요청 사항 파싱
    _request.parseProcess(_req);

    // 2. config 에서 맞는 server block 찾아서 넘기기
    ServerBlock server_block = findServerBlock();
    std::vector<std::string> server_name = server_block.getServerName();
    std::vector<std::string>::iterator server_name_it = server_name.begin();
    std::vector<t_listen> listens = server_block.getListen();
    std::vector<t_listen>::iterator listen_it = listens.begin();

    std::string locationPath;

    server_block = server_block.getLocationForRequest(_request.getTargetPath(),
                                                      locationPath);

    GetConf getConf(_request, server_block, locationPath);
    _response.run(_request, getConf);

    // 4. make response
    _res = _response.getResponse();

    if (_res.empty()) {
        return RETURN_ERROR;
    } else {
        return RETURN_PROCEED;
    }
}

int WebserverProcess::writeResponse(void) {
    // 속도저하
    std::string str = _res.substr(_write_ret_sum, RECV_SIZE);
    int ret = write(_connected_fd, str.c_str(), str.size());
    std::cout << "write result : " << _write_ret_sum << std::endl;

    if (ret == -1) {
        clear();
    } else {
        _write_ret_sum += ret;
        if (_write_ret_sum >= _res.size()) {
            std::cout << "\n --- response ---" << std::endl;
            std::cout << _res.substr(0, 250) << std::endl;
            std::cout << "---------------" << std::endl;
            _res = "";
            _req = "";
            _prev_req_end_index = 0;
            _header_index = 0;
            _ready_to_response = false;
            _write_ret_sum = 0;
            _request.initBody();
            _response
                .initResponse(); //초기화까지는 맞는데 res를 지우면 쓸게 없구나
        }
        ret = 0;
    }
    return ret; // success: 양수, fail: -1
};

void WebserverProcess::clear(void) {
    std::cout << "===> clear (webserver process)" << std::endl;
    if (_socket_fd > 0) {
        close(_socket_fd);
    }
    if (_connected_fd > -0) {
        close(_connected_fd);
    }
    _socket_fd = -1;
    _connected_fd = -1;
}

// util
void WebserverProcess::setAddr(void) {
    int addrlen;
    addrlen = sizeof(_addr);
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = htonl(_listen_info.host);
    _addr.sin_port = htons(_listen_info.port);
};

int WebserverProcess::findKeyIndex(std::string key) {
    int index = _req.find(key);
    if (index == std::string::npos)
        return -1;
    return index;
}

bool WebserverProcess::isFinalChunked(void) {
    int endOfFile = _req.find("0\r\n\r\n");
    if (endOfFile == std::string::npos)
        return false;
    int request_size = _req.size();
    if (request_size != endOfFile + 5)
        return false;
    std::string substr = _req.substr(request_size - 5, 5);
    std::cout << "substr:: " << substr << std::endl;
    if (substr != "0\r\n\r\n")
        return false;
    return true;
};

bool WebserverProcess::listenMatched(std::vector<t_listen> listens) {
    std::vector<t_listen>::iterator it = listens.begin();
    for (; it != listens.end(); it++) {
        if (it->port == _listen_info.port && it->host == _listen_info.host)
            return true;
    }
    return false;
}

void WebserverProcess::decodeChunk() {
    std::string head = _req.substr(0, _req.find("\r\n\r\n"));
    std::string chunks =
        _req.substr(_req.find("\r\n\r\n") + 4, _req.size() - 1);
    std::string subchunk = chunks.substr(0, 100);
    std::string body = "";
    int chunksize = strtol(subchunk.c_str(), NULL, 16);
    size_t i = 0;

    while (chunksize) {
        i = chunks.find("\r\n", i) + 2;
        body += chunks.substr(i, chunksize);
        i += chunksize + 2;
        subchunk = chunks.substr(i, 100);
        chunksize = strtol(subchunk.c_str(), NULL, 16);
        std::cout << " ... process chunked ... " << i << std::endl;
    }
    _req = head + "\r\n\r\n" + body + "\r\n\r\n";
}

ServerBlock WebserverProcess::findServerBlock() {
    // 1. getHeaders에서 host가져오기(request)
    std::map<std::string, std::string> const &header = _request.getHeaders();
    std::string req_host = header.find("Host")->second;
    req_host = req_host.substr(0, req_host.find(':'));
    std::vector<ServerBlock> server_blocks = _config->getServerBlock();
    ServerBlock *ret = NULL;

    // 사전조건: port가 일치하는 것만 후보가 될 수 있음
    // 1.이름이랑 포트 넘버로 먼저 찾아봄 request에 있는 host와 server block의
    // server name을 비교하기(둘다 맞는 걸로 찾음)
    std::cout << "req_host: " << req_host << std::endl;
    std::vector<ServerBlock>::iterator it = server_blocks.begin();
    for (; it != server_blocks.end(); it++) {
        std::vector<std::string> server_name = it->getServerName();
        std::vector<std::string>::iterator server_name_it = server_name.begin();
        for (; server_name_it != server_name.end(); server_name_it++) {
            if (*server_name_it == req_host && listenMatched(it->getListen())) {
                std::cout << "=> find by servername " << *server_name_it
                          << std::endl;
                return *it;
            }
        }
    }
    // 2. 없으면 listen으로 찾아봄(가장 첫번째 일치하는 것으로 결정함-default)
    it = server_blocks.begin();
    for (; it != server_blocks.end(); it++) {
        if (listenMatched(it->getListen())) {
            std::cout << "=> find by ip address - the first one be a default"
                      << std::endl;
            ret = &(*it);
            break;
        }
    }
    return *ret;
}

// getter
int WebserverProcess::getFd(void) { return _socket_fd; };
int WebserverProcess::getConnectedFd(void) { return _connected_fd; };
bool WebserverProcess::getReadyToResponse(void) { return _ready_to_response; };

// occf
WebserverProcess::WebserverProcess(void) {}

WebserverProcess::WebserverProcess(t_listen const &listen, Config &config) {
    _socket_fd = -1;
    _connected_fd = -1;
    _sent = -1;
    _prev_req_end_index = 0;
    _header_index = 0;
    _listen_info = listen;
    _ready_to_response = false;
    _config = &config;
    _write_ret_sum = 0;
    this->setAddr();
}
WebserverProcess::WebserverProcess(WebserverProcess const &src) { *this = src; }
WebserverProcess::~WebserverProcess(void) {}
WebserverProcess &WebserverProcess::operator=(WebserverProcess const &src) {
    _socket_fd = src._socket_fd;
    _connected_fd = src._connected_fd;
    _listen_info = src._listen_info;
    _ready_to_response = src._ready_to_response;
    _addr = src._addr;

    _req = src._req;
    _request = src._request;
    _res = src._res;
    _response = src._response;
    _sent = src._sent;
    _prev_req_end_index = 0;
    _header_index = 0;
    _write_ret_sum = src._write_ret_sum;
    _config = src._config;
    return (*this);
}
