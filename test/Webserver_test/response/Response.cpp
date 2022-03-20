
#include "Response.hpp"

Response::Response() {
}

void Response::run(Request request, ServerBlock serverblock) {
    std::cout << "// response => run //" << std::endl;
}
std::string Response::getResponse() {
    std::cout << "// response => get response //" << std::endl;

    return "HTTP/1.1 200 OK\r\n\
Content-Length: 30\r\n\
Content-Location: /index.html\r\n\
Content-Type: text/html\r\n\
Date: Fri, 04 Mar 2022 09:28:09 GMT\r\n\
Last-Modified: Sun, 13 Feb 2022 09:27:39 GMT\r\n\
Server: Weebserv/1.0.0 (Unix)\r\n\
Transfer-Encoding: identity\r\n\
            This is the default index yo !";
}