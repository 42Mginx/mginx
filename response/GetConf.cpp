#include "GetConf.hpp"

// requestconfig

GetConf::GetConf() {}

GetConf::GetConf(Request &request, ServerBlock &server_block,
                 std::string &locationName) {
    _error_page = server_block.getErrorPage(); // 에러페이지 위치
    _client_body_buffer_size =
        server_block.getClientBodyBufferSize(); // Conf에서 제한한 Client max
                                                // body size, defaults to 8 000

    _cgi_pass = server_block.getCgiPass(); // CGI 위치

    _cgi_param = server_block.getCgiParam();
    if (request.getEnv().begin() != request.getEnv().end())
        _cgi_param = request.getEnv();

    _allowed_methods =
        server_block.getAllowedMethods();     // allowed http METHOD for request
    _autoindex = server_block.getAutoIndex(); // Autoindex FLAG
    std::string alias = server_block.getAlias(); //기본폴더 위치 바꾸기
    std::string root = server_block.getRoot(); // config 에서 기본폴더위치
    std::string ret;
    _redirect = server_block.getRedirect();

    // index 체크
    std::vector<std::string> temp_index = server_block.getIndex();
    for (std::vector<std::string>::const_iterator it = temp_index.begin();
         it != temp_index.end(); it++) {
        std::vector<std::string>::const_iterator it2;
        for (it2 = _index.begin(); it2 != _index.end(); it2++) {
            if (*it == *it2)
                break;
        }
        //겹치는게 없을 때 넣기, 겹치는게 있으면 끝이 아니라서 push하지않음
        if (it2 == _index.end())
            _index.push_back(*it);
    }

    // alias, location 없는상태
    std::string req_path = request.getTargetPath();
    if (locationName[0] != '*' && server_block.getAlias() != "") {
        ret = alias + req_path.substr(locationName.length());
        _content_location = req_path.substr(locationName.length());
        _target_path = removeNearSlashes(ret);
    } else {
        ret = root + request.getTargetPath();
        _content_location = removeNearSlashes(request.getTargetPath());
        _target_path = removeNearSlashes(ret);
    }

    if (_target_path == "./YoupiBanane/")
        _target_path = _target_path + "index.html";
    if (_target_path == "./YoupiBanane")
        _target_path = _target_path + "/index.html";

    if (!pathIsFile(_target_path) && request.getMethod() == "GET") {
        int i = 0;
        for (std::vector<std::string>::const_iterator it = _index.begin();
             it != _index.end(); it++) {
            i++;
        }
        addIndex(request);
    }
}

GetConf::~GetConf() {}

// tester
void GetConf::getConfTester() { std::cout << "getConfTester" << std::endl; }

// getters
const std::string &GetConf::getContentLocation() const {
    return this->_content_location;
}

const std::string &GetConf::getTargetPath() const { return this->_target_path; }

const std::map<int, std::string> &GetConf::getErrorPage() const {
    return this->_error_page;
}

const unsigned long &GetConf::getClientBodyBufferSize() const {
    return this->_client_body_buffer_size;
}

const std::map<std::string, std::string> &GetConf::getCgiParam() const {
    return this->_cgi_param;
}

const std::string &GetConf::getCgiPass() const { return this->_cgi_pass; }

const std::set<std::string> &GetConf::getAllowedMethods() const {
    return this->_allowed_methods;
}

// const t_listen							&GetConf::getHostPort()
// const { 	return this->_hostPort;
// }

bool GetConf::getAutoIndex() const { return this->_autoindex; }

const std::string &GetConf::getRedirect() const { return this->_redirect; }

// Utils

std::string GetConf::removeNearSlashes(const std::string &str) {
    std::string ret;
    bool lastIsSlash = false;

    for (std::string::size_type i = 0; i < str.length(); i++) {
        if (str[i] == '/') {
            if (!lastIsSlash)
                ret.push_back(str[i]);
            lastIsSlash = true;
        } else {
            lastIsSlash = false;
            ret.push_back(str[i]);
        }
    }
    return ret;
}

int GetConf::pathIsFile(const std::string &path) {
    struct stat s;
    if (stat(path.c_str(), &s) == 0) // s에다가 저장해두기
    {
        if (s.st_mode & S_IFDIR) //폴더면 0
            return 0;
        else if (s.st_mode & S_IFREG) // regular file이면 1
            return 1;
        else
            return 0;
    } else
        return 0;
}

std::string GetConf::addIndex(Request &request) {
    std::vector<std::string>::iterator it;
    // 	std::list<std::pair<std::string, float> >::const_iterator	lang;
    std::string path;

    // 	it = this->_index.begin();
    // 	while(it != this->_index.end()) // Check with language prefs
    // 	{
    // 		for (lang = request.getLang().begin(); lang != request.getLang().end();
    // lang++)
    // 		{
    // 			path = this->_path;
    // 			if (path[path.size() - 1] != '/')
    // 				path += "/";
    // 			if ((*it).find('.') != (*it).npos)
    // 				path += (*it).substr(0, (*it).find_last_of('.') + 1) + lang->first +
    // (*it).substr((*it).find_last_of('.')); 			if (pathIsFile(path))
    // 			{
    // 				this->_path = path;
    // 				if (this->_contentLocation.size() &&
    // this->_contentLocation[this->_contentLocation.size() - 1] != '/')
    // 					this->_contentLocation += "/";
    // 				// NOT PROTECTED AGAINST INDEXES WITHOUT
    // EXTENSION 				if ((*it).find('.') != (*it).npos) 					this->_contentLocation +=
    // (*it).substr(0, (*it).find_last_of('.') + 1) + lang->first +
    // (*it).substr((*it).find_last_of('.')); 				return this->_path;
    // 			}
    // 		}
    // 		it++;
    // 	}

    it = this->_index.begin();
    while (it != this->_index.end()) // check with index file only
    {
        path = this->_target_path;
        if (path[path.size() - 1] != '/')
            path += "/";
        path += *it;
        if (pathIsFile(path)) {

            this->_target_path = path;
            if (this->_content_location[this->_content_location.size() - 1] !=
                '/')
                this->_content_location += "/";
            this->_content_location += *it;
            return this->_target_path;
        }
        it++;
    }
    return "";
}
