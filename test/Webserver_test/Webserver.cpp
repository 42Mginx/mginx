#include "Webserver.hpp"
#include "WebserverProcess.hpp"
void Webserver::init() {
    max_fd = 0;
    FD_ZERO(&_fd_set);
    FD_ZERO(&_reading_set);
    FD_ZERO(&_writing_set);

    if (!socket_fd_v.empty())
        socket_fd_v.clear();
    if (!connected_fd_v.empty())
        connected_fd_v.clear();
    if (!process_v.empty())
        process_v.clear();
    if (!listens_v.empty())
        listens_v.clear();
    listens_v = getListens(); // config.getAllListens();
}

int Webserver::setup(void) {
    std::vector<t_listen>::const_iterator listen_i = listens_v.begin();
    for (; listen_i != listens_v.end(); listen_i++) {
        WebserverProcess process(*listen_i);

        if (process.setup() == -1) {
            std::cout << "process setup error" << std::endl;
            break;
        }
        process_v.push_back(process);
        int socket_fd = process.getFd();
        FD_SET(socket_fd, &_fd_set);
        std::cout << socket_fd << std::endl;
        if (socket_fd > max_fd)
            max_fd = socket_fd;
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
            process_it = process_v.begin();
            for (; process_it != process_v.end(); process_it++) {
                bool ready_to_response = process_it->getReadyToResponse();
                if (ready_to_response == true) {
                    int connected_fd = process_it->getConnectedFd();
                    FD_SET(connected_fd, &_writing_set);
                    if (max_fd < connected_fd)
                        max_fd = connected_fd;
                }
            }
            ret = select(max_fd + 1, &_reading_set, &_writing_set, NULL,
                         &timeout);
            std::cout << "기다리는 중입니다." << ret << std::endl;
        }

        std::cout << ret << std::endl;
        if (ret == -1) {
            std::cout << "===> select error!!" << std::endl;
            return -1;
        }

        // write
        process_it = process_v.begin();
        for (; process_it != process_v.end(); process_it++) {
            bool ready_to_response = process_it->getReadyToResponse();
            int connected_fd = process_it->getConnectedFd();
            if (ready_to_response == true &&
                FD_ISSET(connected_fd, &_writing_set)) {
                if (process_it->writeResponse() == -1) {
                    handle_error("write");
                    return -1;
                }
                break;
            }
        };

        // read
        process_it = process_v.begin();
        for (; process_it != process_v.end(); process_it++) {
            int connected_fd = process_it->getConnectedFd();
            if (FD_ISSET(connected_fd, &_reading_set)) {
                FD_CLR(connected_fd, &_fd_set);
                if (process_it->readRequest() == -1) {
                    std::cout << "read 에러" << std::endl;
                    return -1;
                }
                break;
            }
        };

        // accept
        process_it = process_v.begin();
        for (; process_it != process_v.end(); process_it++) {
            int socket_fd = process_it->getFd();
            if (FD_ISSET(socket_fd, &_reading_set)) {
                if (process_it->accept() == -1) {
                    std::cout << "accept 에러" << std::endl;
                    return -1;
                }
                int connected_fd = process_it->getConnectedFd();
                FD_SET(connected_fd, &_fd_set);
                if (max_fd < connected_fd)
                    max_fd = connected_fd;
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
    process_it = process_v.begin();
    for (; process_it != process_v.end(); process_it++) {
        process_it->clear();
    }
    // 2 멤버 변수 초기화

    init();
    setup();
}

// util

std::vector<t_listen> Webserver::getListens() {

    std::vector<t_listen> listens_v;

    t_listen listen_info;
    listen_info.port = 8000;
    listen_info.host = 0;

    t_listen listen_info2;
    listen_info2.port = 8001;
    listen_info2.host = 0;

    listens_v.push_back(listen_info);
    listens_v.push_back(listen_info2);

    return listens_v;
};

// occf
Webserver::Webserver(void) { init(); }

Webserver::Webserver(const Webserver &src) { *this = src; }
Webserver::~Webserver(void) {}
Webserver &Webserver::operator=(const Webserver &src) { return (*this); }
