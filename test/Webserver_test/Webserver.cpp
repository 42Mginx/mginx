#include "Webserver.hpp"

#include "WebserverProcess.hpp"
void Webserver::init() {
    _max_fd = 0;
    FD_ZERO(&_fd_set);
    FD_ZERO(&_reading_set);
    FD_ZERO(&_writing_set);

    if (!_socket_fd_v.empty())
        _socket_fd_v.clear();
    if (!_connected_fd_v.empty())
        _connected_fd_v.clear();
    if (!_process_v.empty())
        _process_v.clear();
    if (!_listens_v.empty())
        _listens_v.clear();
    _listens_v = getListens();  // config.getAllListens();
}

int Webserver::setup(void) {
    std::vector<t_listen>::const_iterator listen_i = _listens_v.begin();
    for (; listen_i != _listens_v.end(); listen_i++) {
        WebserverProcess process(*listen_i);

        if (process.setup() == -1) {
            std::cout << "process setup error" << std::endl;
            break;
        }
        _process_v.push_back(process);
        int socket_fd = process.getFd();
        FD_SET(socket_fd, &_fd_set);
        std::cout << socket_fd << std::endl;
        if (socket_fd > _max_fd)
            _max_fd = socket_fd;
    }
    std::cout << "setting 완료" << std::endl;
    return 0;
};

int Webserver::run() {
    while (1) {
        int ret = 0;
        std::vector<WebserverProcess>::iterator process_it;

        while (ret == 0) {
            struct timeval timeout;
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;
            FD_ZERO(&_writing_set);
            memcpy(&_reading_set, &_fd_set, sizeof(_fd_set));
            process_it = _process_v.begin();
            for (; process_it != _process_v.end(); process_it++) {
                bool ready_to_response = process_it->getReadyToResponse();
                if (ready_to_response == true) {
                    std::cout << "make writing set" << std::endl;
                    int connected_fd = process_it->getConnectedFd();
                    std::cout << connected_fd << std::endl;
                    FD_SET(connected_fd, &_writing_set);
                    if (_max_fd < connected_fd)
                        _max_fd = connected_fd;
                }
            }
            ret = select(_max_fd + 1, &_reading_set, &_writing_set, NULL,
                         &timeout);
            std::cout << "기다리는 중입니다." << ret << std::endl;
        }

        std::cout << "while문 탈출" << std::endl;
        if (ret == -1) {
            std::cout << "===> select error!!" << std::endl;
            return -1;
        }

        // write
        std::cout << "write 진입전" << std::endl;
        process_it = _process_v.begin();
        for (; process_it != _process_v.end(); process_it++) {
            bool ready_to_response = process_it->getReadyToResponse();
            int connected_fd = process_it->getConnectedFd();
            if (ready_to_response == true &&
                connected_fd > 0 &&
                FD_ISSET(connected_fd, &_writing_set)) {
                std::cout << "write" << std::endl;
                if (process_it->writeResponse() == -1) {
                    handle_error("write");
                    return -1;
                }
                break;
            }
        };

        // read
        std::cout << "read 진입전" << std::endl;

        process_it = _process_v.begin();
        std::cout << "=> 1" << std::endl;
        for (; process_it != _process_v.end(); process_it++) {
            std::cout << "=> 2" << std::endl;
            int connected_fd = process_it->getConnectedFd();
            std::cout << "=> 3" << std::endl;
            if (connected_fd > 0 && FD_ISSET(connected_fd, &_reading_set)) {
                std::cout << "read 실행" << std::endl;
                FD_CLR(connected_fd, &_fd_set);
                if (process_it->readRequest() == -1) {
                    std::cout << "read 에러" << std::endl;
                    return -1;
                }
                break;
            }
            std::cout << "=> 4" << std::endl;
        };

        // accept
        std::cout << "accept 진입전" << std::endl;

        process_it = _process_v.begin();
        for (; process_it != _process_v.end(); process_it++) {
            int socket_fd = process_it->getFd();
            if (FD_ISSET(socket_fd, &_reading_set)) {
                std::cout << "accept" << std::endl;
                if (process_it->accept() == -1) {
                    std::cout << "accept 에러" << std::endl;
                    return -1;
                }
                int connected_fd = process_it->getConnectedFd();
                FD_SET(connected_fd, &_fd_set);
                if (_max_fd < connected_fd)
                    _max_fd = connected_fd;
                break;
            }
        };
    }
    return 0;
}

void Webserver::handle_error(std::string const &error_message) {
    std::cout << error_message << " 에러" << std::endl;
    // 모든 조건을 초기화함
    // 1 process 초기화
    std::vector<WebserverProcess>::iterator process_it;
    process_it = _process_v.begin();
    for (; process_it != _process_v.end(); process_it++) {
        process_it->clear();
    }
    // 2 멤버 변수 초기화

    init();
    setup();
}

// util

std::vector<t_listen> Webserver::getListens() {
    std::vector<t_listen> _listens_v;

    t_listen listen_info;
    listen_info.port = 8000;
    listen_info.host = 0;

    t_listen listen_info2;
    listen_info2.port = 8001;
    listen_info2.host = 0;

    _listens_v.push_back(listen_info);
    _listens_v.push_back(listen_info2);

    return _listens_v;
};

// occf
Webserver::Webserver(void) { init(); }

Webserver::Webserver(const Webserver &src) { *this = src; }
Webserver::~Webserver(void) {}
Webserver &Webserver::operator=(const Webserver &src) { return (*this); }
