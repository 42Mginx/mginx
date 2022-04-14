#include "GetConf.hpp"

//requestconfig

GetConf::GetConf()
{}

GetConf::GetConf(Request &request, ServerBlock &server_block, std::string &locationName)
{
		_error_page = server_block.getErrorPage(); // 에러페이지 위치
		_client_body_buffer_size = server_block.getClientBodyBufferSize(); // Conf에서 제한한 Client max body size, defaults to 8 000

		_cgi_pass = server_block.getCgiPass(); // CGI 위치
		// std::cout<<"getconf cgipass : "<<_cgi_pass<<std::endl; 0406

		_cgi_param = server_block.getCgiParam();
		if(request.getEnv().begin() != request.getEnv().end())
			_cgi_param = request.getEnv();

		_allowed_methods = server_block.getAllowedMethods(); // allowed http METHOD for request
		// t_listen							_hostPort;

		//아래 두 멤버는 요청의 끝자리가 '/'일때만 필요, autoindex는 해당파일이 존재하지않을대도 필요
		// _index = server_block.getIndex();
		_autoindex = server_block.getAutoIndex(); // Autoindex FLAG

		// std::cout<<"autoindex : "<<_autoindex<<std::endl; 0406

		std::string	alias = server_block.getAlias(); //기본폴더 위치 바꾸기
		// std::cout<<"alias : "<<alias<<std::endl; 0406
		std::string	root = server_block.getRoot(); //config 에서 기본폴더위치
		std::string	ret;

		/* index 출력 테스트용
		int i = 0;
		for (std::vector<std::string>::const_iterator it = conf_index.begin();it != conf_index.end(); it++) {
			std::cout<<"conf index : "<<conf_index.at(i)<<std::endl;
			i++;
		}
		*/
		std::cout<<"@@@@debug bodysize : "<<server_block.getClientBodyBufferSize()<<"@@"<<std::endl;
		std::cout<<"@@@@debug redirect : "<< server_block.getRedirect() <<"@@"<<std::endl;

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

		// std::cout<<"locationName"<<locationName<<std::endl; 0406
		//alias, location 없는상태
		std::string req_path = request.getTargetPath();
		// std::cout<<"getAliasSet : "<<server_block.getAliasSet()<<std::endl;
		if(locationName[0] != '*' && server_block.getAlias() !="")
		{
			ret = alias + req_path.substr(locationName.length());
			_content_location = req_path.substr(locationName.length());
			_target_path = removeNearSlashes(ret);
		}
		else{
		ret = root + request.getTargetPath();
		_content_location = removeNearSlashes(request.getTargetPath());
		_target_path = removeNearSlashes(ret);
		}

		if(_target_path == "./YoupiBanane/")
		_target_path = _target_path +"index.html";
		if(_target_path == "./YoupiBanane")
		_target_path = _target_path +"/index.html";


		// if (!pathIsFile(this->_target_path) && request.getMethod() == "GET") {
		// if (request.getTargetPath() == "/" && request.getMethod() == "GET") {
		// 	_content_location =	server_block.getIndex().at(0);
		// 	ret = root +"/"+ _content_location;
		// 	_target_path = removeNearSlashes(ret);
		// }

		if (!pathIsFile(_target_path) && request.getMethod() == "GET")
		{
		// /* index 출력 테스트용
		int i = 0;
		for (std::vector<std::string>::const_iterator it = _index.begin();it != _index.end(); it++) {
			// std::cout<<"conf index : "<<_index.at(i)<<std::endl; 0406
			i++;
		}
		// std::cout<<"client body size : "<<server_block.getClientBodyBufferSize()<<std::endl; 0406

		// */

			addIndex(request);
		}
		// 	if(_index.at(0) != "")
		// // 	{
		// 		_content_location =	server_block.getIndex().at(0);
		// 		ret = root +"/"+ _content_location;
		// 		_target_path = removeNearSlashes(ret);
		// 	}
		// }

		// std::cout<<"request_target_path : "<<request.getTargetPath()<<std::endl; 0406
		// std::cout<<"target_path : "<<_target_path<<std::endl; 0406

		// _target_path = removeNearSlashes("dummy.html");//Path는 진짜 파일경로
		// _content_location = "dummy.html"; // 실제 대상 파일 위치

		/*추가작업해야할 것들


		*/

	// std::string indexPath;
	// if (!pathIsFile(this->_path) && method == "GET") {
	// 	std::cout<<"index path@@@@@@@@@@@@@@@@"<<std::endl;
	// 	//indexPath가 공백이 아니면?
	// 	indexPath = this->addIndex(request);
	// 	std::cout<<indexPath<<std::endl;

	// 	// if ((indexPath = this->addIndex(request)) != "") {
	// 	// 	config = GetConf.getLocationForRequest(indexPath, locationName);
	// 	// 	this->_cgi_pass = get.getCgiPass();
	// 	// 	this->_cgi_param = config.getCgiParam();
	// 	// 	std::cout<<_cgi_pass<<std::endl;
	// 	// }
	// }

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

const std::string&  									GetConf::getLocation() const {
	return this->_location;
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


int		GetConf::pathIsFile(const std::string& path)
{
	struct stat s;
	if (stat(path.c_str(), &s) == 0 ) //s에다가 저장해두기
	{
		if (s.st_mode & S_IFDIR) //폴더면 0
			return 0;
		else if (s.st_mode & S_IFREG) //regular file이면 1
			return 1;
		else
			return 0;
	}
	else
		return 0;
}


std::string								GetConf::addIndex(Request& request)
{
	std::vector<std::string>::iterator							it;
// 	std::list<std::pair<std::string, float> >::const_iterator	lang;
	std::string													path;

// 	it = this->_index.begin();
// 	while(it != this->_index.end()) // Check with language prefs
// 	{
// 		for (lang = request.getLang().begin(); lang != request.getLang().end(); lang++)
// 		{
// 			path = this->_path;
// 			if (path[path.size() - 1] != '/')
// 				path += "/";
// 			if ((*it).find('.') != (*it).npos)
// 				path += (*it).substr(0, (*it).find_last_of('.') + 1) + lang->first + (*it).substr((*it).find_last_of('.'));
// 			if (pathIsFile(path))
// 			{
// 				this->_path = path;
// 				if (this->_contentLocation.size() && this->_contentLocation[this->_contentLocation.size() - 1] != '/')
// 					this->_contentLocation += "/";
// 				// NOT PROTECTED AGAINST INDEXES WITHOUT EXTENSION
// 				if ((*it).find('.') != (*it).npos)
// 					this->_contentLocation += (*it).substr(0, (*it).find_last_of('.') + 1) + lang->first + (*it).substr((*it).find_last_of('.'));
// 				return this->_path;
// 			}
// 		}
// 		it++;
// 	}

	it = this->_index.begin();
	while(it != this->_index.end()) // check with index file only
	{
		path = this->_target_path;
		if (path[path.size() - 1] != '/')
			path += "/";
		path += *it;
		if (pathIsFile(path))
		{

			this->_target_path = path;
			if (this->_content_location[this->_content_location.size() - 1] != '/')
				this->_content_location += "/";
			this->_content_location += *it;
			return this->_target_path;
		}
		it++;
	}
	return "";
}
