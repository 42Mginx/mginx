#include <iostream>
#include <string>
#include "Request.hpp"

int main(void)
{
	std::string request_value;

	request_value = "POST / HTTP/1.1 \
					Host: localhost:8000 \
					User-Agent: Go-http-client/1.1\
					Transfer-Encoding: chunked\
					Content-Type: test/file\
					Accept-Encoding: gzip"\;
	
	Request test(request_value);

}