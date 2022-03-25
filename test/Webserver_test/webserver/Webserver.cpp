#include "Webserver.hpp"

#include "WebserverProcess.hpp"

void Webserver::parseConfig(std::string config_path) {
    _config.parseProcess(config_path);
    std::cout << "testing// after parse conf" << std::endl;
}

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
    _listens_v = _config.getAllListens();
    std::cout << "init 완료" << std::endl;
    if (_listens_v.empty()) {
        std::cout << "listens is empty" << std::endl;
    }
}

int Webserver::setup(void) {
    std::vector<t_listen>::const_iterator listen_i = _listens_v.begin();
    if (_listens_v.empty()) {
        std::cout << "listens is empty" << std::endl;
    }
    for (; listen_i != _listens_v.end(); listen_i++) {
        WebserverProcess process(*listen_i, _config);
        std::cout << listen_i->host << ": " << listen_i->port << std::endl;
        if (process.setup() == -1) {
            std::cerr << RED << "Could not bind [" << listen_i->port << "]"
                      << RESET << std::endl;
        } else {
            std::cout << GREEN << "Bind success [" << listen_i->port << "]"
                      << RESET << std::endl;
        }
        _process_v.push_back(process);
        int socket_fd = process.getFd();
        FD_SET(socket_fd, &_fd_set);
        if (socket_fd > _max_fd)
            _max_fd = socket_fd;
    }
    std::cout << YELLOW << " ----------------- setting 완료 ---------------- "
              << RESET << std::endl;
    return 0;
};

int Webserver::run() {
    while (1) {
        int ret = 0;
        std::vector<WebserverProcess>::iterator process_it;
        size_t flag = 1;

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
                    FD_SET(connected_fd, &_writing_set);
                    if (_max_fd < connected_fd)
                        _max_fd = connected_fd;
                }
            }
            ret = select(_max_fd + 1, &_reading_set, &_writing_set, NULL,
                         &timeout);
            if (flag == 1) {
                std::cout << "\r[" << ret << "] ....waiting...." << &std::flush;
                flag = 0;
            } else {
                std::cout << "\r[" << ret << "] ..for change..." << &std::flush;
                flag = 1;
            }
        }

        std::cout << "\r! something changed !" << std::endl;
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
            if (ready_to_response == true && connected_fd > 0 &&
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
        for (; process_it != _process_v.end(); process_it++) {
            int connected_fd = process_it->getConnectedFd();
            if (connected_fd > 0 && FD_ISSET(connected_fd, &_reading_set)) {
                std::cout << "read 실행" << std::endl;
                FD_CLR(connected_fd, &_fd_set);
                int result = process_it->readRequest();
                if (result == -1) {
                    std::cout << "read 에러" << std::endl;
                    return -1;
                }
                break;
            }
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

// occf
Webserver::Webserver(void) { init(); }

Webserver::Webserver(const Webserver &src) { *this = src; }
Webserver::~Webserver(void) {}
Webserver &Webserver::operator=(const Webserver &src) { return (*this); }
