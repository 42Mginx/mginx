#include "Webserver.hpp"
#include "WebserverProcess.hpp"

void Webserver::run() {
    long max_fd = 0;

    // fd_set 생성, 초기화
    fd_set _fd_set;
    FD_ZERO(&_fd_set);

    // 파싱된 info에서 받아와야함. 아마도 config?
    std::vector<t_listen> listens_v = getListens();

    std::vector<int> socket_fd_v;
    std::vector<int> connected_fd_v;
    std::vector<WebserverProcess> process_v;

    // socket 생성 => 반환값: socket fd
    std::vector<t_listen>::const_iterator listen_i = listens_v.begin();
    for (; listen_i != listens_v.end(); listen_i++) {

        WebserverProcess process(*listen_i);

        process.setup();
        process_v.push_back(process);

        int socket_fd = process.getFd();
        // socket fd를 fd_set에서 켜줌
        FD_SET(socket_fd, &_fd_set);
        std::cout << socket_fd << std::endl;

        // max fd 갱신
        if (socket_fd > max_fd)
            max_fd = socket_fd;
    }
    std::cout << "setting 완료" << std::endl;

    // ============================ [run] ===========================
    fd_set _reading_set;
    fd_set _writing_set;

    while (1) {

        int ret = 0;

        // FD_ZERO(&_writing_set);
        // = > 아직 필요없음(res 보낼 때 필요)

        while (ret == 0) {
            struct timeval timeout;
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;

            memcpy(&_reading_set, &_fd_set, sizeof(_fd_set));
            ret = select(max_fd + 1, &_reading_set, NULL, NULL,
                         &timeout); // WRITING SET 설정필요함
            std::cout << "기다리는 중입니다." << ret
                      << std::endl; // -1이면 에러임
        }

        std::cout << ret << std::endl; // -1이면 에러임 => 에러처리 필요
        if (ret == -1) {
            std::cout << "===> error!!" << std::endl;
        }

        std::vector<WebserverProcess>::iterator process_it = process_v.begin();
        for (; process_it != process_v.end(); process_it++) {
            int socket_fd = process_it->getFd();
            if (FD_ISSET(socket_fd, &_reading_set)) {
                if (process_it->accept() == -1) {
                    std::cout << "accept 에러" << std::endl;
                }
                break;
            }
        };

        std::vector<WebserverProcess>::iterator process_it2 = process_v.begin();
        for (; process_it2 != process_v.end(); process_it2++) {
            int connected_fd = process_it2->getConnectedFd();
            if (connected_fd != -1) {
                std::cout << "read" << std::endl;
                if (process_it2->readRequest() == -1) {
                    std::cout << "read 에러" << std::endl;
                }
                process_it2->readyToResponse();
                break;
            }
        };

        std::vector<WebserverProcess>::iterator process_it3 = process_v.begin();
        for (; process_it3 != process_v.end(); process_it3++) {
            bool ready_to_response = process_it3->getReadyToResponse();
            std::cout << "write" << std::endl;
            if (ready_to_response == true) {
                if (process_it3->writeResponse() == -1) {
                    std::cout << "write 에러" << std::endl;
                }
                break;
            }
        };
    }
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
    // Wait, that's illegal !
}
Webserver::Webserver(const Webserver &src) { *this = src; }
Webserver::~Webserver(void) {}
Webserver &Webserver::operator=(const Webserver &src) { return (*this); }
