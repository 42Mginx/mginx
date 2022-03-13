#include <iostream>
#include <string>
#include "Request.hpp"

int main(void)
{
	std::string request_value;

	request_value = "POST /directory?id=3&hi=2 HTTP/1.1 \r\n\
\r\n\
Host: localhost:8000 \r\n\
dongslee: baboo\r\n\
User-Agent: Go-http-client/1.1\r\n\
Transfer-Encoding: chunked\r\n\
Content-Type: test/file\r\n\
Accept-Encoding: gzip\r\n\
\r\n\
ddd\r\n\
ssdsdsd\r\n\
sdsdsdsd";
	
	Request test(request_value);
	return (0);
}