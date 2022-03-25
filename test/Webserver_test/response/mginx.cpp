#include "Webserver.hpp"
#include "Response.hpp"
#include "request/Request.hpp"
#include "GetConf.hpp"

int main()
{
	Response response;
	Request request;
	ServerBlock serverblock;

	//reqest, serverblock은 만들어져 있는 상태
	//mergeRequestServerblcok만들기

	/*
	getConf 세팅하기
	1. serverBlock찾기
	2. serverblock내부에서 location찾기
	3. if(location)
		location 설정 있으면 설정 바꿔주기
	4. GetConf 생성자로 초기세팅해주기
	*/

	//requestConf = conf.getConfigForRequest(this->_listen,  request.getPath(),
	//                 request.getHeaders().at("Host"), request.getMethod(), request);
	//RequestConfig config(server, request, uri, method, locationPath);
	//1. serverblock
	//2. request
	//3. uri
	//4. request method 넣어주기

	//request.getHeaders().at("Host"), request.getMethod() 이거 두개는 굳이 인자로 넣을 필요없을듯?
	//locationPath는 locationName -> root + location 할예정, 파일찾을 때 사용


	//serverblock 은 찾아서 넘겨줄예정
	//dummy 서버블록 만들어서 테스트하기
 	GetConf getConf(request,serverblock);
	response.run(request, getConf);
	std::cout<<response.getResponse();
}


