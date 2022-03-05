#include <iostream>
#include <string>
#include "Request.hpp"

int main(void)
{
	std::string request_value;

	request_value = "POST /directory HTTP/1.1\r\n\
Host: localhost:8000 \r\n\
User-Agent: Go-http-client/1.1\r\n\
Transfer-Encoding: chunked\r\n\
Content-Type: test/file\r\n\
Accept-Encoding: gzip\r\n";
	
	Request test(request_value);

	std::cout << test.getMethod() << std::endl;
	std::cout << test.getTargetPath() << std::endl;
	std::cout << test.getVersion() << std::endl;

	return (0);
}