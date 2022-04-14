#include "WebserverProcess.hpp"

#include <time.h>

// 소켓 fd 생성부터 listen까지
int WebserverProcess::setup(void)
{
    // 소켓 fd 만듦
    _socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket_fd != -1)
    {
        // fcntl(_socket_fd, F_SETFL, O_NONBLOCK);
        if (bind(_socket_fd, (struct sockaddr *)&_addr, sizeof(_addr)) == -1)
        {
            return -1;
        }
        // fd 소켓 열어줌.
        listen(_socket_fd, 1000);
    }
    // 에러 시 -1 리턴 아닐 시 소켓 fd 리턴
    return _socket_fd;
};

int WebserverProcess::accept(void)
{
    _connected_fd = ::accept(_socket_fd, NULL, NULL);
    std::cout << "===> accept(WebserverProcess) " << _socket_fd << ":" << _connected_fd << std::endl;
    if (_connected_fd != -1)
    {
        fcntl(_connected_fd, F_SETFL, O_NONBLOCK);
    }
    return _connected_fd; // success: fd, fail: -1
};

int WebserverProcess::readRequest(void)
{
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

    if (_header_index == 0 || _header_index == std::string::npos)
    {
        _header_index = _req.find("\r\n\r\n"); //*수정
    }
    _prev_req_end_index += ret - 1;
    std::cout << "read now: " << _prev_req_end_index << std::endl;
    if (_header_index != std::string::npos)
    {
        if (_req.find("Content-Length: ") == std::string::npos)
        {
            if (_req.find("Transfer-Encoding: chunked") != std::string::npos)
            {
                // chuncked 인데 다 받았거나, chuncked가 아니면 0 => PROCEED
                // std::cout << "this is not chunked, or it's final one"
                if (isFinalChunked())
                {
                    ret = RETURN_PROCEED;
                }
                else
                    return (RETURN_WAIT);
            }
            else
            {
                ret = RETURN_PROCEED;
            }
        }

        std::string key = "Content-Length: ";
        int content_len_index = findKeyIndex(key);
        if (content_len_index != -1)
        {
            size_t body_size = std::atoi(_req.substr(content_len_index + key.size(), 10).c_str());
            size_t total_size = body_size + _header_index + 4;
            if (_req.size() >= total_size)
            {
                std::cout << "prepared to proceed" << std::endl;
                ret = RETURN_PROCEED;
            }
            else
            {
                std::cout << "need to wait more req" << std::endl;
                return (RETURN_WAIT);
            }
        }
    }
    else
        return (RETURN_WAIT); // 0406
    // std::cerr << "// header not found //" << std::endl;

    if (ret == RETURN_PROCEED)
        ret = process();
    if (ret == RETURN_ERROR)
    {
        std::cerr << "// empty response error //" << std::endl;
        _ready_to_response = false;
        return RETURN_ERROR; // -1
    }
    else if (ret == RETURN_PROCEED)
    {
        std::cout << "// res is ready... //" << std::endl;
        _ready_to_response = true;
    }
    // //*추가 0406
    // if (ret == RETURN_WAIT)
    //     _ready_to_response = false;

    //*
    return ret; // success: 1(RETURN_WAIT)/0(RETURN_PROCEED)
};

int WebserverProcess::process(void)
{
    // 0. chuncked 뒤에 chundked가 온 경우
    int chunked_index = _req.find("Transfer-Encoding: chunked");
    if (chunked_index != std::string::npos && chunked_index < _req.find("\r\n\r\n"))
        decodeChunk();

    // 1. 요청 사항 파싱
    _request.parseProcess(_req);

    // 2. config 에서 맞는 server block 찾아서 넘기기
    ServerBlock server_block = findServerBlock();

    // ========= check ========
    std::cout << "check server block" << std::endl;
    std::vector<std::string> server_name = server_block.getServerName();
    std::vector<std::string>::iterator server_name_it = server_name.begin();
    if (server_name_it == server_name.end())
    {
        std::cout << "??????" << std::endl;
    }
    for (; server_name_it != server_name.end(); server_name_it++)
    {
        std::cout << "==> server names: " << *server_name_it << std::endl;
    }
    std::vector<t_listen> listens = server_block.getListen();
    std::vector<t_listen>::iterator listen_it = listens.begin();
    for (; listen_it != listens.end(); listen_it++)
    {
        std::cout << "==> listnes: " << listen_it->host << "," << listen_it->port << std::endl;
    }

    std::cout << "webserver process request_target_path : " << _request.getTargetPath() << std::endl;


    clock_t start, finish;
    double duration;

    std::string locationPath;

    start = clock();

    server_block = server_block.getLocationForRequest(_request.getTargetPath(), locationPath);

    finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    std::string filename("response.txt");
    std::ofstream file_out;
    file_out.open(filename, std::ios_base::app);
    file_out<<"Time Stamp[getLocationForRequest ] : "<<duration<<std::endl;
    file_out.close();


    start = clock();

    GetConf getConf(_request, server_block, locationPath);

    finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    file_out.open(filename, std::ios_base::app);
    file_out<<"Time Stamp[Getconf ] : "<<duration<<std::endl;
    file_out.close();

    start = clock();

    _response.run(_request, getConf);

     finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    file_out.open(filename, std::ios_base::app);
    file_out<<"Time Stamp[response.run] : "<<duration<<std::endl;
    file_out.close();
    // 4. make response


    start = clock();

    std::cout<<"debug3"<<std::endl;
    _res = _response.getResponse();

    // _res = "HTTP/1.1 301 OK\r\nContent-Length: 2\r\nContent-Location: http://naver.com\r\nLocation: abc/abc\r\nContent-Type: text/html\r\nDate: Wed, 13 Apr 2022 17:22:04 GMT\r\nLast-Modified: Tue, 12 Apr 2022 05:43:54 GMT\r\nServer: Mginx/1.0.0\r\n\r\nt\r\n\r\n";


     finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    file_out.open(filename, std::ios_base::app);
    file_out<<"Time Stamp[response.getResonse ] : "<<duration<<std::endl;
    file_out.close();




    if (_res.empty())
    {
        return RETURN_ERROR;
    }
    else
    {
        return RETURN_PROCEED;
    }
}

int WebserverProcess::writeResponse(void)
{
    // std::cout << "===> write" << std::endl;
    //         _res =
    //             "HTTP/1.1 405 Method Not Allowed\r\n\
    // Date: Fri, 01 Apr 2022 07:05:38 GMT\r\n\
    // Server: Mginx/1.0.0\r\n\
    // Content-Length: 0\r\n\
    // Content-Location: /\r\n\
    // Content-Type: text/plain\r\n\
    // Last-Modified: Tue, 19 Oct 2021 05:22:24 GMT\r\n\
    // \r\n";

    // std::cout << "this is odd:" << _res << std::endl; //0406 임시삭제 속도저하
    std::string str = _res.substr(_write_ret_sum, RECV_SIZE); // 0406 수정
    int ret = write(_connected_fd, str.c_str(), str.size());  // 0406 수정
    // int ret = write(_connected_fd, _res.c_str(), _res.size());
    std::cout << "write result : " << _write_ret_sum << std::endl; // 0406 임시삭제 속도저하

    if (ret == -1)
    {
        clear();
    }
    // wirte과정에서 body가 많을 때 모아서 써줘야 함
    else
    {
        _write_ret_sum += ret;
        // std::cout<<"ret_sum : "<<_sum<<" _res.size"<<_res.size()<<std::endl;

        // std::string filename("response.txt");
        // std::ofstream file_out;
        // file_out.open(filename, std::ios_base::app);
        // file_out<<"response : "<<_res<<std::endl;
        // file_out.close();

        if (_write_ret_sum >= _res.size())
        {
            std::cout << "\n===> write" << std::endl;

            // res 쓰기
            std::string filename("response.txt");
            std::ofstream file_out;
            file_out.open(filename, std::ios_base::app);
            file_out << "response: " << _res.substr(0, 2000) << std::endl;
            file_out.close();

            _res = "";
            _req = "";

            _prev_req_end_index = 0;
            _header_index = 0;
            _ready_to_response = false;
            _write_ret_sum = 0;
            _request.initBody();
            _response.initResponse(); //초기화까지는 맞는데 res를 지우면 쓸게 없구나
        }
        ret = 0;
    }
    return ret; // success: 양수, fail: -1
};

void WebserverProcess::clear(void)
{
    std::cout << "===> clear (webserver process)" << std::endl;
    if (_socket_fd > 0)
    {
        close(_socket_fd);
    }
    if (_connected_fd > -0)
    {
        close(_connected_fd);
    }
    _socket_fd = -1;
    _connected_fd = -1;
}

// util
void WebserverProcess::setAddr(void)
{
    int addrlen;

    addrlen = sizeof(_addr);
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = htonl(_listen_info.host);
    _addr.sin_port = htons(_listen_info.port);
};

int WebserverProcess::findKeyIndex(std::string key)
{
    int index = _req.find(key);
    if (index == std::string::npos)
        return -1;
    return index;
}

bool WebserverProcess::isFinalChunked(void)
{
    int endOfFile = _req.find("0\r\n\r\n");
    if (endOfFile == std::string::npos)
        return false;
    int request_size = _req.size();
    if (request_size != endOfFile + 5)
        return false;
    // 이거 왜 하는 거지?
    std::string substr = _req.substr(request_size - 5, 5);
    std::cout << "substr:: " << substr << std::endl;
    if (substr != "0\r\n\r\n")
        return false;
    return true;
};

bool WebserverProcess::listenMatched(std::vector<t_listen> listens)
{
    std::vector<t_listen>::iterator it = listens.begin();
    for (; it != listens.end(); it++)
    {
        if (it->port == _listen_info.port && it->host == _listen_info.host)
        {
            std::cout << "listen matched!!(" << _listen_info.port << "," << _listen_info.host << ")" << std::endl;
            return true;
        }
    }
    return false;
}

void WebserverProcess::decodeChunk()
{
    std::string head = _req.substr(0, _req.find("\r\n\r\n"));
    std::string chunks = _req.substr(_req.find("\r\n\r\n") + 4, _req.size() - 1);
    std::string subchunk = chunks.substr(0, 100);
    std::string body = "";
    int chunksize = strtol(subchunk.c_str(), NULL, 16);
    size_t i = 0;

    while (chunksize)
    {
        i = chunks.find("\r\n", i) + 2;
        body += chunks.substr(i, chunksize);
        i += chunksize + 2;
        subchunk = chunks.substr(i, 100);
        chunksize = strtol(subchunk.c_str(), NULL, 16);
        std::cout << " ... process chunked ... " << i << std::endl;
    }
    _req = head + "\r\n\r\n" + body + "\r\n\r\n";
}

ServerBlock WebserverProcess::findServerBlock()
{
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
    for (; it != server_blocks.end(); it++)
    {
        std::vector<std::string> server_name = it->getServerName();
        std::vector<std::string>::iterator server_name_it = server_name.begin();
        for (; server_name_it != server_name.end(); server_name_it++)
        {
            if (*server_name_it == req_host && listenMatched(it->getListen()))
            {
                std::cout << "=> find by servername " << *server_name_it << std::endl;
                return *it;
            }
        }
    }
    // 2. 없으면 listen으로 찾아봄(가장 첫번째 일치하는 것으로 결정함-default)
    it = server_blocks.begin();
    for (; it != server_blocks.end(); it++)
    {
        if (listenMatched(it->getListen()))
        {
            std::cout << "=> find by ip address - the first one be a default" << std::endl;
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

// 값들 초기화 하고, 받은 listen과 config 받음, 받은 listen 구조체로 addr설정
WebserverProcess::WebserverProcess(t_listen const &listen, Config &config)
{
    _socket_fd = -1;
    _connected_fd = -1;
    _sent = -1;
    _prev_req_end_index = 0;
    _header_index = 0;
    _listen_info = listen;
    _ready_to_response = false;
    _config = &config;
    _write_ret_sum = 0; // 0406 추가
    this->setAddr();
}
WebserverProcess::WebserverProcess(WebserverProcess const &src) { *this = src; }
WebserverProcess::~WebserverProcess(void) {}
WebserverProcess &WebserverProcess::operator=(WebserverProcess const &src)
{
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
    _write_ret_sum = src._write_ret_sum; // 0406 추가
    _config = src._config;
    return (*this);
}
