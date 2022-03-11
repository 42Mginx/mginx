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
    std::string choice;

    std::cout << YELLOW << "Choose PORT :" << std::endl;
    std::cout << "(a) 8000" << std::endl;
    std::cout << "(b) 8001" << std::endl;
    // std::cout << "(c) 8002" << RESET << std::endl;
    getline(std::cin, choice);

    if (choice == "a")
        return (8000);
    else if (choice == "b")
        return (8001);
    // else if (choice == "c")
    //     return (8002);
    return (8000);
}

std::string choose_method(void) {
    std::string choice;

    std::cout << std::endl
              << YELLOW << "Choose METHOD :" << std::endl;
    std::cout << "(a) GET" << std::endl;
    std::cout << "(b) HEAD" << std::endl;
    std::cout << "(c) POST" << std::endl;
    std::cout << "(d) PUT" << std::endl;
    std::cout << "(e) DELETE" << std::endl;
    std::cout << "(f) OPTIONS" << std::endl;
    std::cout << "(g) TRACE" << RESET << std::endl;
    getline(std::cin, choice);

    if (choice == "a")
        return ("get/GET");
    else if (choice == "b")
        return ("head/HEAD");
    else if (choice == "c")
        return ("post/POST");
    else if (choice == "d")
        return ("put/PUT");
    else if (choice == "e")
        return ("delete/DELETE");
    else if (choice == "f")
        return ("options/OPTIONS");
    else if (choice == "g")
        return ("trace/TRACE");
    return ("get/GET");
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

void send(int port) {
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[BUF_SIZE] = {
        0,
    };
    // std::fstream		file;
    std::string content;

    // file.open(filename);
    // content.assign(std::istreambuf_iterator<char>(file),
    // std::istreambuf_iterator<char>()); file.close();

    sock = socket(AF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    int ret = connect(sock, (struct sockaddr *)&serv_addr,
                      sizeof(serv_addr));  // 내부에서 2번 악수
    std::cout << "connect " << ret << std::endl;
    std::string request_string = "invalid request";

    if (port == 8000) {
        request_string =
            "GET /directory/youpi.bla HTTP/1.1\r\n\r\n\
Host: localhost:8000\r\n\
User-Agent: Go-http-client/1.1\r\n\
Accept-Encoding: gzip\r\n\
";
    } else if (port == 8001) {
        request_string =
            "POST /directory?id=3&hi=2 HTTP/1.1 \r\n\r\n\
Host: localhost:8001 \r\n\
User-Agent: Go-http-client/1.1\r\n\
Transfer-Encoding: chunked\r\n\
Content-Type: test/file\r\n\
Accept-Encoding: gzip\r\n\
\r\n\
4\r\n\
onit\r\n\
";
    }
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

int main(void) {
    int port = choose_port();
    std::cout << "port: " << port << std::endl;
    send(port);
    // send(8000);
    return (0);
}
