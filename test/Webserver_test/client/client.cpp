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

#define GET 1
#define POST 2
#define DELETE 3

#define BUF_SIZE 4096

int choose_port(void) {
    std::string input;

    std::cout << YELLOW << "[ Choose PORT ]" << std::endl;
    std::cout << "1) 8000" << std::endl;
    std::cout << "2) 8001" << RESET << std::endl;
    getline(std::cin, input);

    if (input == "1") {
        return (8000);
    } else if (input == "2") {
        return (8001);
    } else {
        return (8000);
    }
}

int choose_method(void) {
    std::string input;

    std::cout << std::endl
              << YELLOW << "[ Choose METHOD ]" << std::endl;
    std::cout << "1) GET" << std::endl;
    std::cout << "2) POST" << std::endl;
    std::cout << "3) DELETE" << RESET << std::endl;
    getline(std::cin, input);

    if (input == "1")
        return (GET);
    else if (input == "2")
        return (POST);
    else if (input == "3")
        return (DELETE);
    return (GET);
}

std::string choose_option(int method) {
    std::string input;

    std::cout << std::endl
              << YELLOW << "[ Choose the OPTION ]" << std::endl;
    std::cout << "1) Simple" << std::endl;
    std::cout << "2) Bad request" << RESET << std::endl;
    if (method != DELETE) {
        std::cout << YELLOW << "3) CGI" << RESET << std::endl;
    }
    if (method == POST) {
        std::cout << YELLOW << "4) Chunked" << RESET << std::endl;
    }

    getline(std::cin, input);

    if (input == "1")
        return ("_simple");
    else if (input == "2")
        return ("_bad");
    else if (method != DELETE && input == "3")
        return ("_cgi");
    else if (method == POST && input == "4")
        return ("_chunked");

    return ("_simple");
}

void send(int port, std::string filename) {
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[BUF_SIZE] = {
        0,
    };
    std::fstream file;
    std::string request_string;

    // get req string
    file.open(filename);
    request_string.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    file.close();
    request_string += "\r\n";
    std::cout << "req: [" << request_string << "]" << std::endl;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "// Client socket creation failed //" << std::endl;
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    int ret = connect(sock, (struct sockaddr*)&serv_addr,
                      sizeof(serv_addr));  // 내부에서 2번 악수
    if (ret < 0) {
        std::cerr << "// Connection failed //" << std::endl;
    }
    std::cout << "=> connect " << ret << std::endl;

    send(sock, request_string.c_str(), request_string.length(), 0);
    std::cout << "=> send" << std::endl;

    ret = read(sock, buffer, BUF_SIZE - 1);
    std::cout << ret << std::endl;

    std::cout << std::endl
              << "Response :" << std::endl;
    std::cout << "[" << std::string(buffer) << "]" << std::endl
              << std::endl;

    close(sock);
    return;
}

std::string get_filename(int method, std::string option) {
    if (method == GET) {
        return "get/GET" + option;
    } else if (method == POST) {
        return "post/POST" + option;
    } else if (method == DELETE) {
        return "delete/DELETE" + option;
    }
    return "get/GET_simple";
}

int main(int argc, char** argv) {
    int port;
    int method;
    std::string option;
    std::string filename = "test/request_examples/";

    while (1) {
        port = choose_port();
        method = choose_method();
        option = choose_option(method);
        filename += get_filename(method, option);
        std::cout << filename << std::endl;
        send(port, filename);
    }

    return (0);
}
