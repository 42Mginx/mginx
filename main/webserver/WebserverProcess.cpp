#include "WebserverProcess.hpp"

// action
int WebserverProcess::setup(void) {
    _socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket_fd != -1) {
        fcntl(_socket_fd, F_SETFL, O_NONBLOCK);
        if (bind(_socket_fd, (struct sockaddr *)&_addr, sizeof(_addr)) == -1) {
            return -1;
        }
        listen(_socket_fd, 1000);
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
        std::cout << "read error!" << std::endl;
        return -1;
    }

    _req += std::string(buffer);

    size_t header_index = _req.find("\r\n");
    std::cout << header_index << std::endl;
    if (header_index != std::string::npos) {
        bool chuncked = isChunked();
        if (!chuncked || (chuncked && isFinalChunked())) {
            // chuncked 인데 다 받았거나, chuncked가 아니면 0 => PROCEED
            std::cout << "this is not chunked, or it's final one"
                      << isFinalChunked() << std::endl;
            ret = RETURN_PROCEED;
        } else {
            // chuncked 인데 아직 덜 받았으면 return 1 => WAIT
            std::cout << "this is chunked, and it's not final one" << std::endl;
            ret = RETURN_WAIT;
        }
        std::string key = "Content-Length: ";
        int content_len_index = findKeyIndex(key);
        if (content_len_index != -1) {
            size_t body_size = std::atoi(_req.substr(content_len_index + key.size(), 10).c_str());
            size_t total_size = body_size + header_index + 4;
            if (_req.size() >= total_size) {
                std::cout << "prepared to proceed" << std::endl;
                ret = RETURN_PROCEED;
            } else {
                std::cout << "need to wait more req" << std::endl;
                ret = RETURN_WAIT;
            }
        }
    } else {
        std::cerr << "// header not found //" << std::endl;
    }

    if (ret == RETURN_PROCEED) {
        ret = process();
    }
    std::cout << "request is [" << _req << "]" << std::endl;

    if (ret == RETURN_ERROR) {
        std::cerr << "// empty response error //" << std::endl;
        _ready_to_response = false;
        return RETURN_ERROR;  // -1
    }
    if (ret == RETURN_PROCEED) {
        std::cout << "// res is ready... //" << std::endl;
        _ready_to_response = true;
    }
    return ret;  // success: 0(RETURN_WAIT)/1(RETURN_PROCEED)
};

int WebserverProcess::process(void) {
    // 0. chuncked 뒤에 chundked가 온 경우
    int chunked_index = _req.find("Transfer-Encoding: chunked");
    if (chunked_index != std::string::npos && chunked_index < _req.find("\r\n\r\n")) {
        decodeChunk();

        // if (chunked_body != "") {
        std::cout << "request: " << _req << std::endl;
        // }
    }

    // 1. 요청 사항 파싱
    _request.parseProcess(_req);

    // 2. config 에서 맞는 server block 찾아서 넘기기
    ServerBlock server_block = findServerBlock();

    // ========= check ========
    std::cout << "check server block" << std::endl;
    std::vector<std::string> server_name = server_block.getServerName();
    std::vector<std::string>::iterator server_name_it = server_name.begin();
    if (server_name_it == server_name.end()) {
        std::cout << "??????" << std::endl;
    }
    for (; server_name_it != server_name.end(); server_name_it++) {
        std::cout << "==> server names: " << *server_name_it << std::endl;
    }
    std::vector<t_listen> listens = server_block.getListen();
    std::vector<t_listen>::iterator listen_it = listens.begin();
    for (; listen_it != listens.end(); listen_it++) {
        std::cout << "==> listnes: " << listen_it->host << "," << listen_it->port << std::endl;
    }

    _response.run(_request, server_block);
    // 3. make response
    _res = _response.getResponse();
    if (_res.empty()) {
        return RETURN_ERROR;
    } else {
        return RETURN_PROCEED;
    }
}

int WebserverProcess::writeResponse(void) {
    std::cout << "===> write" << std::endl;

    int ret = write(_connected_fd, _res.c_str(), _res.size());
    _connected_fd = -1;
    _ready_to_response = false;
    _req = "";
    return ret;  // success: 양수, fail: -1
};

void WebserverProcess::clear(void) {
    std::cout << "===> clear (webserver process)" << std::endl;
    if (_socket_fd > 0) {
        close(_socket_fd);
    }
    if (_connected_fd > 0) {
        close(_connected_fd);
    }
}

// util
void WebserverProcess::setAddr(void) {
    int addrlen;
    
    addrlen = sizeof(_addr);
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = _listen_info.host;
    // _addr.sin_addr.s_addr = _listen_info.host || htonl(INADDR_ANY);
    std::cout << "--------host_start---------" << std::endl;
        std::cout << inet_addr("1.0.0.1") << std::endl;
    std::cout << inet_addr("127.0.0.1") << std::endl;
    std::cout << _listen_info.host << std::endl;
    std::cout << INADDR_ANY << std::endl;
    std::cout << "-----listen_host_end-----" << std::endl;
    _addr.sin_port = htonl(_listen_info.port);
};

int WebserverProcess::findKeyIndex(std::string key) {
    int index = _req.find(key);
    if (index == std::string::npos)
        return -1;
    return index;
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
    std::cout << "substr:: " << substr << std::endl;
    if (substr != "0\r\n\r\n")
        return false;
    return true;
};

bool WebserverProcess::listenMatched(std::vector<t_listen> listens) {
    std::vector<t_listen>::iterator it = listens.begin();
    for (; it != listens.end(); it++) {
        if (it->port == _listen_info.port && it->host == _listen_info.host) {
            std::cout << "listen matched!!(" << _listen_info.port << "," << _listen_info.host << ")" << std::endl;
            return true;
        }
    }
    return false;
}

void WebserverProcess::decodeChunk() {
    std::string chunk = _req;
    std::string head = chunk.substr(0, chunk.find("\r\n\r\n"));
    std::cout << "~~~> head: " << head << std::endl;
    chunk = chunk.substr(head.length() + 4);
    std::cout << "~~~> chunks: " << chunk << std::endl;

    std::string body = "";
    size_t chunk_size = 0;
    size_t read_size = 1;

    while (chunk != "" && read_size != 0) {
        std::string chunk_size_str = chunk.substr(0, chunk.find("\r\n"));
        chunk_size = strtol(chunk_size_str.c_str(), NULL, 16);
        std::cout << "~~~> chunk_size: " << chunk_size << std::endl;
        chunk = chunk.substr(chunk_size_str.length() + 2);

        if (chunk_size == 0) {
            std::cout << chunk_size_str.length() << std::endl;
            break;
        }

        std::string chunk_body = "";
        size_t sub_read_size = 0;
        while (chunk_size > chunk_body.length()) {
            std::cout << "chunk_size" << chunk_size << std::endl;
            std::cout << "chunk_body.length()" << chunk_body.length() << std::endl;
            std::string sub_chunk_body = chunk.substr(0, chunk.find("\r\n"));

            std::cout << "~~~> sub_chunk_body: " << sub_chunk_body << std::endl;
            chunk = chunk.substr(sub_chunk_body.length() + 2);
            std::cout << "~~~> 남은 chunks: " << chunk << std::endl;
            chunk_body += sub_chunk_body;
        }
        std::cout << "~~~> chunk_body: " << chunk_body << std::endl;
        read_size = chunk_body.length();
        body += chunk_body;
    }

    std::cout << "==========>decoded body: [" << body << "]" << std::endl;
    if (chunk_size == 0 && chunk != "\r\n") {
        std::cerr << "There is body to read => [" << chunk << "]" << std::endl;
    }
    if (body == "") {
        _req = head + "\r\n\r\n";

    } else {
        _req = head + "\r\n\r\n" + body + "\r\n\r\n";
    }
};

ServerBlock WebserverProcess::findServerBlock() {
    // 1. getHeaders에서 host가져오기(request)
    std::map<std::string, std::string> const &header = _request.getHeaders();
    std::string req_host = header.find("Host")->second;
    req_host = req_host.substr(0, req_host.find(':'));
    std::vector<ServerBlock> server_blocks = _config->getServerBlock();
    ServerBlock *ret = NULL;

    // 사전조건: port가 일치하는 것만 후보가 될 수 있음
    // 1.이름이랑 포트 넘버로 먼저 찾아봄 request에 있는 host와 server block의 server name을 비교하기(둘다 맞는 걸로 찾음)
    std::cout
        << "req_host: "
        << req_host
        << std::endl;
    std::vector<ServerBlock>::iterator it = server_blocks.begin();
    for (; it != server_blocks.end(); it++) {
        std::vector<std::string> server_name = it->getServerName();
        std::vector<std::string>::iterator server_name_it = server_name.begin();
        for (; server_name_it != server_name.end(); server_name_it++) {
            if (*server_name_it == req_host && listenMatched(it->getListen())) {
                std::cout << "=> find by servername " << *server_name_it << std::endl;
                return *it;
            }
        }
    }
    // 2. 없으면 listen으로 찾아봄(가장 첫번째 일치하는 것으로 결정함-default)
    it = server_blocks.begin();
    for (; it != server_blocks.end(); it++) {
        if (listenMatched(it->getListen())) {
            std::cout << "=> find by ip address - the first one be a default" << std::endl;
            ret = &(*it);
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
    _listen_info = listen;
    _ready_to_response = false;
    _config = &config;
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

    _config = src._config;
    return (*this);
}
