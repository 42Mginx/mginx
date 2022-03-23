// Client side C/C++ program to demonstrate Socket programming
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <fstream>
#include <iostream>

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"

#define BUF_SIZE 4096

int choose_port(void) {
    std::string input;

    std::cout << YELLOW << "Choose PORT :" << std::endl;
    std::cout << "(a) 8000" << std::endl;
    std::cout << "(b) 8001" << RESET << std::endl;
    getline(std::cin, input);

    if (input == "a") {
        return (8000);
    } else if (input == "b") {
        return (8001);
    } else {
        return (8000);
    }
}

std::string choose_method(void) {
    std::string input;

    std::cout << std::endl
              << YELLOW << "Choose METHOD :" << std::endl;
    std::cout << "(a) GET" << std::endl;
    std::cout << "(c) POST" << std::endl;
    std::cout << "(e) DELETE" << RESET << std::endl;
    getline(std::cin, input);

    if (input == "a")
        return ("GET");
    else if (input == "b")
        return ("POST");
    else if (input == "c")
        return ("DELETE");
    return ("GET");
}

std::string choose_target(int cgi) {
    std::string choice;

    std::cout << std::endl
              << YELLOW << "Choose TARGET FILE :" << std::endl;
    std::cout << "(a) Exists" << std::endl;
    std::cout << "(b) Doesn't exist" << std::endl;
    std::cout << "(c) Wrong permissions" << std::endl;
    std::cout << "(d) Bad request" << RESET << std::endl;
    if (cgi)
        std::cout << YELLOW << "(e) CGI (on)" << RESET << std::endl;

    getline(std::cin, choice);

    if (choice == "a")
        return ("_example");
    else if (choice == "b")
        return ("_nofile");
    else if (choice == "c")
        return ("_permission");
    else if (choice == "d")
        return ("_bad");
    else if (cgi && choice == "e")
        return ("_cgi");
    return ("_example");
}

void send(int port, std::string request_string) {
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[BUF_SIZE] = {
        0,
    };

    sock = socket(AF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    int ret = connect(sock, (struct sockaddr*)&serv_addr,
                      sizeof(serv_addr));  // 내부에서 2번 악수
    std::cout << "connect " << ret << std::endl;
    // std::string request_string = "?";

    //     if (port == 8000 && mode == "normal") {
    //         request_string =
    //             "GET / HTTP/1.1\r\n\
// Host: localhost:8000\r\n\
// User-Agent: Go-http-client/1.1\r\n\
// Accept-Encoding: gzip\r\n\
// ";

    //     } else if (port == 8001 && mode == "normal") {
    //         request_string =
    //             "GET / HTTP/1.1\r\n\
// Host: localhost:8001\r\n\
// User-Agent: Go-http-client/1.1\r\n\
// Accept-Encoding: gzip\r\n\
// ";
    //     } else if (port == 8001 && mode == "chunked") {
    //         request_string =
    //             "POST /directory?id=3&hi=2 HTTP/1.1 \r\n\
// Host: localhost:8001 \r\n\
// User-Agent: Go-http-client/1.1\r\n\
// Transfer-Encoding: chunked\r\n\
// Content-Type: test/file\r\n\
// Accept-Encoding: gzip\r\n\
// \r\n\
// 4\r\n\
// abcd\r\n\
// 3\r\n\
// abc\r\n\
// 0\r\n\
// \r\n\
// ";
    //     } else if (mode == "cs") {
    //         request_string =
    //             "POST /directory?id=3&hi=2 HTTP/1.1 \r\n\
// Host: localhost:8000 \r\n\
// User-Agent: Go-http-client/1.1\r\n\
// Transfer-Encoding: chunked\r\n\
// Content-Type: test/file\r\n\
// Accept-Encoding: gzip\r\n\
// \r\n\
// 3\r\n\
// abc\r\n\
// \r\n\
// ";
    //     } else if (mode == "ce") {
    //         request_string =
    //             "POST /directory?id=3&hi=2 HTTP/1.1 \r\n\
// Host: localhost:8000 \r\n\
// User-Agent: Go-http-client/1.1\r\n\
// Transfer-Encoding: chunked\r\n\
// Content-Type: test/file\r\n\
// Accept-Encoding: gzip\r\n\
// \r\n\
// 3\r\n\
// def\r\n\
// 0\r\n\
// \r\n\
// ";
    //     }

    std::cout << "req: [" << request_string << "]" << std::endl;
    send(sock, request_string.c_str(), request_string.length(), 0);
    std::cout << "send" << std::endl;

    ret = read(sock, buffer, BUF_SIZE - 1);
    std::cout << ret << std::endl;

    std::cout << std::endl
              << "Response :" << std::endl;
    std::cout << "[" << std::string(buffer) << "]" << std::endl
              << std::endl;

    close(sock);
    return;
}

int main(int argc, char** argv) {
    int port;
    std::string req_string;
    std::string method;

    while (1) {
        port = choose_port();
        std::cout << "=> port : " << port << std::endl;
        method = choose_method();
        std::cout << "=> method : " << method << std::endl;
        // send();
    }
    // chunk
    //     send(8001,
    //          "POST /directory?id=3&hi=2 HTTP/1.1 \r\n\
// Host: localhost:8001 \r\n\
// User-Agent: Go-http-client/1.1\r\n\
// Transfer-Encoding: chunked\r\n\
// Content-Type: test/file\r\n\
// Accept-Encoding: gzip\r\n\
// \r\n\
// 8\r\n\
// abcdefgh\r\n\
// 3\r\n\
// hi\r\n\
// j\r\n\
// 0\r\n\
// \r\n\
// ");
    //     send(8001,
    //          "POST /directory?id=3&hi=2 HTTP/1.1 \r\n\
// Host: localhost:8001 \r\n\
// User-Agent: Go-http-client/1.1\r\n\
// Transfer-Encoding: chunked\r\n\
// Content-Type: test/file\r\n\
// Accept-Encoding: gzip\r\n\
// \r\n\
// 4\r\n\
// abcd\r\n\
// 3\r\n\
// \r\n\
// e\r\n\
// f\r\n\
// g\r\n\
// \r\n\
// \r\n\
// \r\n\
// \r\n\
// \r\n\
// 0\r\n\
// \r\n\
// ");

    //     send(8000,
    //          "GET / HTTP/1.1\r\n\
// Host: localhost:8000\r\n\
// User-Agent: Go-http-client/1.1\r\n\
// Accept-Encoding: gzip\r\n\
//         \r\n");
    //     send(8000,
    //          "POST / HTTP/1.1\r\n\
// Host: localhost:8000\r\n\
// User-Agent: Go-http-client/1.1\r\n\
// Transfer-Encoding: chunked\r\n\
// Content-Type: test/file\r\n\
// Accept-Encoding: gzip\r\n\
// \r\n\
// 0\r\n\
// \r\n\
// ");
    //     send(
    //         8000,
    //         "GET /directory HTTP/1.1\r\n\
// Host: localhost:8000\r\n\
// User-Agent: Go-http-client/1.1\r\n\
// Accept-Encoding: gzip\r\n\
//             \r\n");
    //     send(8000,
    //          "GET /directory/youpi.bad_extension HTTP/1.1\r\n\
// Host: localhost:8000\r\n\
// User-Agent: Go-http-client/1.1\r\n\
// Accept-Encoding: gzip\r\n\
//             \r\n");
    //     send(8000,
    //          "GET /directory/youpi.bla HTTP/1.1\r\n\
// Host: localhost:8000\r\n\
// User-Agent: Go-http-client/1.1\r\n\
// Accept-Encoding: gzip\r\n\
//             \r\n");
    //     send(8000,
    //          "GET /directory/oulalala HTTP/1.1\r\n\
// Host: localhost:8000\r\n\
// User-Agent: Go-http-client/1.1\r\n\
// Accept-Encoding: gzip\r\n\
//             \r\n");

    return (0);
}
