#include "Webserver.hpp"

#include <errno.h>

#include "WebserverProcess.hpp"

void Webserver::parseConfig(std::string config_path)
{
    _config.parseProcess(config_path);
}

void Webserver::init()
{
    _max_fd = 0;
    FD_ZERO(&_fd_set);
    FD_ZERO(&_reading_set);
    FD_ZERO(&_writing_set);

    // if (!_socket_fd_v.empty())
    //     _socket_fd_v.clear();
    // if (!_connected_fd_v.empty())
    //     _connected_fd_v.clear();
    if (!_process_v.empty())
        _process_v.clear();
    if (!_socket_v.empty())
        _socket_v.clear();
    if (!_listens_v.empty())
        _listens_v.clear();
    _listens_v = _config.getAllListens();
}

int Webserver::setup(void)
{
    std::vector<t_listen>::const_iterator listen_i = _listens_v.begin();
    for (; listen_i != _listens_v.end(); listen_i++)
    {
        WebserverProcess process(*listen_i, _config);
        std::cout << listen_i->host << ": " << listen_i->port << std::endl;

        if (process.setup() == -1)
        {
            std::cout << "에러번호" << errno << ": " << strerror(errno) << std::endl;
            std::cerr << RED << "Could not bind [" << listen_i->port << "]"
                      << RESET << std::endl;
        }
        else
        {
            std::cout << GREEN << "Bind success [" << listen_i->port << "/" << process.getFd() << "]"
                      << RESET << std::endl;
            // _process_v.push_back(process);
            _socket_v.push_back(process); // [2022-04-10] 1. port마다 소켓 생성
        }
        int socket_fd = process.getFd();
        FD_SET(socket_fd, &_fd_set);
        if (socket_fd > _max_fd)
            _max_fd = socket_fd;
    }
    std::cout << YELLOW << " ----------------- setting 완료 ---------------- "
              << RESET << std::endl;
    return 0;
};

int Webserver::run()
{
    int connected_fd;

    while (1)
    {
        int ret = 0;
        std::vector<WebserverProcess>::iterator process_it;
        size_t flag = 1;

        while (ret == 0)
        {
            struct timeval timeout;
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;
            FD_ZERO(&_writing_set);
            memcpy(&_reading_set, &_fd_set, sizeof(_fd_set));
            for (process_it = _process_v.begin(); process_it != _process_v.end(); process_it++)
            {
                bool ready_to_response = process_it->getReadyToResponse();
                if (ready_to_response == true)
                {
                    std::cout << "\nmake writing set" << std::endl;
                    connected_fd = process_it->getConnectedFd();
                    FD_SET(connected_fd, &_writing_set);
                    if (_max_fd < connected_fd)
                        _max_fd = connected_fd;
                }
            }
            if (flag == 1)
            {
                std::cout << "\r[" << _max_fd << "] ....waiting...." << &std::flush;
                flag = 0;
            }
            else
            {
                std::cout << "\r[" << _max_fd << "] ..for change..." << &std::flush;
                flag = 1;
            }

            // 		std::string filename("response.txt");
            // std::ofstream file_out;
            // file_out.open(filename, std::ios_base::app);
            // file_out<<"_max_fd : "<<_max_fd<<std::endl;
            // file_out.close();

            ret = select(_max_fd + 1, &_reading_set, &_writing_set, NULL,
                         &timeout);
        }

        if (ret > 0)
        {
            // write
            // std::cout << "write 진입전" << std::endl;
            for (process_it = _process_v.begin(); ret && process_it != _process_v.end(); process_it++)
            {
                bool ready_to_response = process_it->getReadyToResponse();
                connected_fd = process_it->getConnectedFd();
                if (ready_to_response == true && connected_fd > 0 &&
                    FD_ISSET(connected_fd, &_writing_set))
                {
                    // std::cout << "write 진입" << std::endl;
                    if (process_it->writeResponse() == -1)
                    {
                        handle_error("write error");
                        return -1;
                    }
                    ret = 0;
                    // _process_v.erase(process_it);  // [2022-04-10] 4. process_v에서 삭제
                    // FD_CLR(connected_fd, &_fd_set);
                    break;
                }
            };

            // read
            // std::cout << "read 진입전" << std::endl;
            for (process_it = _process_v.begin(); ret && process_it != _process_v.end(); process_it++)
            {
                connected_fd = process_it->getConnectedFd();
                if (connected_fd > 0 && FD_ISSET(connected_fd, &_reading_set))
                {
                    // std::cout << "read 진입" << std::endl;
                    int result = process_it->readRequest();
                   if (result == RETURN_CLOSE) {
                        close(connected_fd);
                        FD_CLR(connected_fd, &_fd_set);
                        FD_CLR(connected_fd, &_reading_set);
                    }
                    if (result == RETURN_ERROR) {
                        std::cout << "read 에러: " << strerror(errno)
                                  << std::endl;
                        FD_CLR(connected_fd, &_fd_set);
                        FD_CLR(connected_fd, &_reading_set);
                        // return -1;
                    }
                    ret = 0;
                    break;
                }
            };

            // accept
            // std::cout << "accept 진입전" << std::endl;

            for (process_it = _socket_v.begin(); ret && process_it != _socket_v.end(); process_it++)
            {
                int socket_fd = process_it->getFd();
                if (FD_ISSET(socket_fd, &_reading_set))
                {
                    WebserverProcess process(*process_it); //[2022-04-10] 2. process 소켓 생성
                    std::cout << "accept 진입" << std::endl;
                    if (process.accept() == -1)
                    {
                        std::cout << "accept 에러" << std::endl;
                        return -1;
                    }
                    connected_fd = process.getConnectedFd();
                    _process_v.push_back(process); // [2022-04-10] 3. process_v에 추가
                    FD_SET(connected_fd, &_fd_set);
                    if (_max_fd < connected_fd)
                        _max_fd = connected_fd;
                    ret = 0;
                    break;
                }
            };
        }
        else
        {
            handle_error("===> select error!!");
            return -1;
        }
    }
    return 0;
}

void Webserver::handle_error(std::string const &error_message)
{
    std::cout << error_message << " 에러" << std::endl;
    std::vector<WebserverProcess>::iterator process_it;
    process_it = _process_v.begin();
    for (; process_it != _process_v.end(); process_it++)
    {
        process_it->clear();
    }
    init();
    setup();
}

// occf
Webserver::Webserver(void) {}

Webserver::Webserver(const Webserver &src) { *this = src; }
Webserver::~Webserver(void) {}
Webserver &Webserver::operator=(const Webserver &src) { return (*this); }
