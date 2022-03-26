#include "GetConf.hpp"

//requestconfig

GetConf::GetConf()
{}

GetConf::GetConf(Request &request, ServerBlock &server_block )
{


		_error_page = server_block.getErrorPage(); // 에러페이지 위치
		_client_body_buffer_size = server_block.getClientBodyBufferSize(); // Conf에서 제한한 Client max body size, defaults to 8 000

		_cgi_pass = server_block.getCgiPass(); // CGI 위치
		_cgi_param = server_block.getCgiParam();
		_allowed_methods = server_block.getAllowedMethods(); // allowed http METHOD for request
		// t_listen							_hostPort;

		//아래 두 멤버는 요청의 끝자리가 '/'일때만 필요, autoindex는 해당파일이 존재하지않을대도 필요
		// _index = server_block.getIndex();
		_autoindex = server_block.getAutoIndex(); // Autoindex FLAG

		std::string	alias = server_block.getAlias(); //기본폴더 위치 바꾸기
		std::string	root = server_block.getRoot(); //config 에서 기본폴더위치
		std::string	ret;

		/* index 출력 테스트용
		int i = 0;
		for (std::vector<std::string>::const_iterator it = conf_index.begin();it != conf_index.end(); it++) {
			std::cout<<"conf index : "<<conf_index.at(i)<<std::endl;
			i++;
		}
		*/

		//index 체크
		std::vector<std::string> temp_index = server_block.getIndex();

		for (std::vector<std::string>::const_iterator it = temp_index.begin();it != temp_index.end(); it++) {
		std::vector<std::string>::const_iterator it2;
		for (it2 = _index.begin(); it2 != _index.end(); it2++) {
				if (*it == *it2)
					break;
			}
		//겹치는게 없을 때 넣기, 겹치는게 있으면 끝이 아니라서 push하지않음
		if (it2 == _index.end())
			_index.push_back(*it);
		}

		// _target_path = removeNearSlashes("dummy.html");//Path는 진짜 파일경로
		_content_location = "dummy.html"; // 실제 대상 파일 위치

		/*추가작업해야할 것들


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
	return this->_content_location;
}

const std::string&							GetConf::getTargetPath() const {
	return this->_target_path;
}

const std::map<int, std::string>&			GetConf::getErrorPage() const {
	return this->_error_page;
}

const unsigned long							&GetConf::getClientBodyBufferSize() const {
	return this->_client_body_buffer_size;
}

const std::map<std::string, std::string>	&GetConf::getCgiParam() const {
	return this->_cgi_param;
}

const std::string&							GetConf::getCgiPass() const {
	return this->_cgi_pass;
}

const std::set<std::string>				&GetConf::getAllowedMethods() const {
	return this->_allowed_methods;
}

// const t_listen							&GetConf::getHostPort() const {
// 	return this->_hostPort;
// }

bool  									GetConf::getAutoIndex() const {
	return this->_autoindex;
}


//Utils

std::string						GetConf::removeNearSlashes(const std::string &str) {
	std::string	ret;
	bool		lastIsSlash = false;

	for (std::string::size_type i = 0; i < str.length(); i++) {
		if (str[i] == '/') {
			if (!lastIsSlash)
				ret.push_back(str[i]);
			lastIsSlash = true;
		}
		else {
			lastIsSlash = false;
			ret.push_back(str[i]);
		}
	}
	return ret;
}
