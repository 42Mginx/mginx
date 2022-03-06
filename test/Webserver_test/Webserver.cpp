#include "Webserver.hpp"
#include "WebserverProcess.hpp"

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

        while (ret == 0) {
            struct timeval timeout;
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;

            memcpy(&_reading_set, &_fd_set, sizeof(_fd_set));
            ret = select(max_fd + 1, &_reading_set, NULL, NULL, &timeout);
            std::cout << "기다리는 중입니다." << ret << std::endl;
        }

        std::cout << ret << std::endl;
        if (ret == -1) {
            std::cout << "===> error!!" << std::endl;
            return -1;
        }

        // accept
        std::vector<WebserverProcess>::iterator process_it = process_v.begin();
        for (; process_it != process_v.end(); process_it++) {
            int socket_fd = process_it->getFd();
            if (FD_ISSET(socket_fd, &_reading_set)) {
                if (process_it->accept() == -1) {
                    std::cout << "accept 에러" << std::endl;
                    return -1;
                }
                break;
            }
        };

        // read
        process_it = process_v.begin();
        for (; process_it != process_v.end(); process_it++) {
            int connected_fd = process_it->getConnectedFd();
            if (connected_fd != -1) {
                if (process_it->readRequest() == -1) {
                    std::cout << "read 에러" << std::endl;
                    return -1;
                }
                process_it->readyToResponse();
                break;
            }
        };

        // write
        process_it = process_v.begin();
        for (; process_it != process_v.end(); process_it++) {
            bool ready_to_response = process_it->getReadyToResponse();
            if (ready_to_response == true) {
                if (process_it->writeResponse() == -1) {
                    std::cout << "write 에러" << std::endl;
                    return -1;
                }
                break;
            }
        };
    }
    return 0;
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
Webserver::Webserver(void) {
    max_fd = 0;
    FD_ZERO(&_fd_set);
    listens_v = getListens(); // config.getAllListens();
}
Webserver::Webserver(const Webserver &src) { *this = src; }
Webserver::~Webserver(void) {}
Webserver &Webserver::operator=(const Webserver &src) { return (*this); }
