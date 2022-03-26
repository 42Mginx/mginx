#include "GetConf.hpp"

//requestconfig

GetConf::GetConf()
{}

GetConf::GetConf(Request &request, ServerBlock &server_block )
{
		_contentLocation = "dummy.html"; // 실제 대상 파일 위치
		_errorPage = server_block.getErrorPage(); // 에러페이지 위치
		_clientBodyBufferSize = server_block.getClientBodyBufferSize(); // Conf에서 제한한 Client max body size, defaults to 8 000

		_cgiPass = server_block.getCgiPass();; // CGI 위치
		_allowedMethods = server_block.getAllowedMethods(); // allowed http METHOD for request
		// t_listen							_hostPort;

		//아래 두 멤버는 요청의 끝자리가 '/'일때만 필요, autoindex는 해당파일이 존재하지않을대도 필요
		_index = server_block.getIndex(); // GET요청에 아무것도 들어 오지않앗을 경우 처리
		_autoindex = server_block.getAutoIndex(); // Autoindex FLAG

		/*추가작업해야할 것들
		_root
		_cgiParam;
		*/


}

GetConf::~GetConf()
{

}

//tester
void GetConf::getConfTester(){
	std::cout<<"getConfTester"<<std::endl;
}

//getters
const std::string&							GetConf::getContentLocation() const {
	return this->_contentLocation;
}

const std::string&							GetConf::getTargetPath() const {
	return this->_targetPath;
}

const std::map<int, std::string>&			GetConf::getErrorPage() const {
	return this->_errorPage;
}

const unsigned long							&GetConf::getClientBodyBufferSize() const {
	return this->_clientBodyBufferSize;
}

const std::map<std::string, std::string>	&GetConf::getCgiParam() const {
	return this->_cgiParam;
}

const std::string&							GetConf::getCgiPass() const {
	return this->_cgiPass;
}

const std::set<std::string>				&GetConf::getAllowedMethods() const {
	return this->_allowedMethods;
}

// const t_listen							&GetConf::getHostPort() const {
// 	return this->_hostPort;
// }

bool  									GetConf::getAutoIndex() const {
	return this->_autoindex;
}
