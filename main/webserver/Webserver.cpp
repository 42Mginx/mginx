#include "Webserver.hpp"

#include <errno.h>

#include "WebserverProcess.hpp"

void Webserver::parseConfig(std::string config_path) {
    _config.parseProcess(config_path);
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
}

int Webserver::setup(void) {
    std::vector<t_listen>::const_iterator listen_i = _listens_v.begin();
    for (; listen_i != _listens_v.end(); listen_i++) {
        // 멤버 변수에 listen이랑 config 넘기면서 WebserverProcess 클래스 생성
        WebserverProcess process(*listen_i, _config);
        // host랑 port 출력
        std::cout << listen_i->host << ": " << listen_i->port << std::endl;

        // 프로세스 셋업 실패시 에러 - 성공시 성공 출력 (소켓 fd 생성부터 bind, listen까지 함)
        if (process.setup() == -1) {
            std::cout << "에러번호" << errno << ": " << strerror(errno) << std::endl;
            std::cerr << RED << "Could not bind [" << listen_i->port << "]"
                      << RESET << std::endl;
        } else {
            std::cout << GREEN << "Bind success [" << listen_i->port << "/" << process.getFd() << "]"
                      << RESET << std::endl;
        }
        // 프로세스 넣어 줌. (의문: 왜 실패한 것도?)
        _process_v.push_back(process);
        // 프로세스 _socket_fd 가져와서 넣어줌.
        int socket_fd = process.getFd();
        // 소켓 fd를 fd_set에 set해줌 -> 일단 지금은 0
        FD_SET(socket_fd, &_fd_set);
        // max_fd 설정(어디까지 읽어야할 지)
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
            timeout.tv_sec = 1; // 초
            timeout.tv_usec = 0; // micro초
            FD_ZERO(&_writing_set);
            // reading_set 
            memcpy(&_reading_set, &_fd_set, sizeof(_fd_set));
            process_it = _process_v.begin();
            // process 돌면서 _ready_to_response 값 받아와서
            // true이면(response줄 준비가 되면) _connected_fd를 wrtting_set에 추가해줌(읽을 준비)
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
            if (flag == 1) {
                std::cout << "\r[" << ret << "] ....waiting...." << &std::flush;
                flag = 0;
            } else {
                std::cout << "\r[" << ret << "] ..for change..." << &std::flush;
                flag = 1;
            }
            ret = select(_max_fd + 1, &_reading_set, &_writing_set, NULL,
                         &timeout);
        }


        // std::cout << "\r! something changed !" << std::endl;
        if (ret < 0) {
            handle_error("===> select error!!");
            return -1;
        }
        if (ret > 0) {
            // write
            // std::cout << "write 진입전" << std::endl;
            process_it = _process_v.begin();
            for (; ret && process_it != _process_v.end(); process_it++) {
                bool ready_to_response = process_it->getReadyToResponse();
                int connected_fd = process_it->getConnectedFd();
                if (ready_to_response == true && connected_fd > 0 &&
                    FD_ISSET(connected_fd, &_writing_set)) {
                    std::cout << "write 진입" << std::endl;
                    if (process_it->writeResponse() == -1) {
                        handle_error("write error");
                        return -1;
                    }
                    ret = 0;
                    break;
                }
            };

            // read
            // std::cout << "read 진입전" << std::endl;

            process_it = _process_v.begin();
            for (; ret && process_it != _process_v.end(); process_it++) {
                int connected_fd = process_it->getConnectedFd();
                if (connected_fd > 0 && FD_ISSET(connected_fd, &_reading_set)) {
                    std::cout << "read 진입" << std::endl;
                    FD_CLR(connected_fd, &_fd_set);
                    int result = process_it->readRequest();
                    if (result == -1) {
                        std::cout << "read 에러" << std::endl;
                        return -1;
                    }
                    ret = 0;
                    break;
                }
            };

            // accept
            // std::cout << "accept 진입전" << std::endl;

            process_it = _process_v.begin();
            for (; ret && process_it != _process_v.end(); process_it++) {
                int socket_fd = process_it->getFd();
                if (FD_ISSET(socket_fd, &_reading_set)) {
                    int connected_fd = process_it->getConnectedFd();
                    std::cout << "accept 진입" << std::endl;
                    if (process_it->accept() == -1) {
                        std::cout << "accept 에러" << std::endl;
                        return -1;
                    }
                    connected_fd = process_it->getConnectedFd();
                    FD_SET(connected_fd, &_fd_set);
                    if (_max_fd < connected_fd)
                        _max_fd = connected_fd;
                    ret = 0;
                    break;
                }
            };
        }
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
Webserver::Webserver(void) {}

Webserver::Webserver(const Webserver &src) { *this = src; }
Webserver::~Webserver(void) {}
Webserver &Webserver::operator=(const Webserver &src) { return (*this); }
