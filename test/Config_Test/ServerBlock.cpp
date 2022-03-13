#include "./ServerBlock.hpp"

/*------------- 캐노니컬  --------------------------------------------------*/

ServerBlock::ServerBlock(): _root(""), _client_body_buffer_size(0), _auto_index(false)
{
	_initDirectivesMap();
};
ServerBlock::~ServerBlock(void) {}
ServerBlock	&ServerBlock::operator=(ServerBlock const &src) {
	if (this != &src) {
		_server_name = src._server_name;
		_listen = src._listen;
		_root = src._root;
		_allowed_methods = src._allowed_methods;
		_error_page = src._error_page;
		_client_body_buffer_size = src._client_body_buffer_size;
		_auto_index = src._auto_index;
		_location = src._location;
	}
	return *this;
}

/*------------- 지시어 파싱 맵과 함수들  --------------------------------------------------*/

void ServerBlock::_initDirectivesMap()
{
	directivesParseFunc["server_name"] = &ServerBlock::addServerName;
	directivesParseFunc["listen"] = &ServerBlock::addListen;
	directivesParseFunc["root"] = &ServerBlock::addRoot;
	directivesParseFunc["error_page"] = &ServerBlock::addErrorPage;
	directivesParseFunc["client_body_buffer_size"] = &ServerBlock::addClientBodyBufferSize;
	directivesParseFunc["autoindex"] = &ServerBlock::addAutoIndex;
	directivesParseFunc["allow_methods"] = &ServerBlock::addAllowedMethods;
	directivesParseFunc["location"] = &ServerBlock::addLocation;
}

void ServerBlock::addServerName(std::vector<std::string> args)
{
	// 에러처리
	if (args.size() == 0)
		throw ServerBlock::ExceptionInvalidArguments();
	// _server_name 값 넣어줌 
	for (unsigned int i = 0; i < args.size(); i++)
		_server_name.push_back(args[i]);
}

void ServerBlock::addListen(std::vector<std::string> args)
{
	t_listen    listen;
	size_t      separator;

	// are는 무조건 1개 아니면 에러
	if (args.size() != 1)
		throw ServerBlock::ExceptionInvalidArguments();
	// ':'가 없을 경우, (+ separator에 ":"대입)
	if ((separator = args[0].find(":")) == std::string::npos) {
		// 첫번째가 숫자면
		if (isDigits(args[0])) {
			// 호스트에 0넣고, port에 args[0] 내용9(8000) 복붙
			listen.host = 0;
			listen.port = atoi(args[0].c_str());
			// 자기 자신 _listen과 파싱한 listen의 port를 비교함
			for (std::vector<t_listen>::const_iterator it = _listen.begin(); it != _listen.end(); it++) {
					// _listen(이미 파싱한 값)과 listen(지금 들어온 값)이 같으면 에러(중복 값)
					if (it->port == listen.port)
						throw ServerBlock::ExceptionInvalidArguments();
			}
			// _listen에 listen 넣음.
			_listen.push_back(listen);
			return ;
		}
		throw ServerBlock::ExceptionInvalidArguments();
	}
	// ':'가 있을 경우 (ip주소 있을 경우)
	else
	{
		// separator까지 주소 파싱해서 listen.host에 uint 포인터로 넣기, localhost도 127.0.0.1로 바꿔줌
		listen.host = strToIp(args[0].substr(0, separator));
		separator++;
		// portStr에 그 이후 값의 주소 값 넣기
		std::string	portStr = args[0].substr(separator);
		// portStr이 숫자이면
		if (isDigits(portStr)) {
			// portStr을 char포인터로 바꿔서 atoi해서 port에 넣기
			listen.port = atoi(portStr.c_str());
			// listen을 값 넣어주기
			_listen.push_back(listen);
			return ;
		}
		throw ServerBlock::ExceptionInvalidArguments();
	}
}

void ServerBlock::addRoot(std::vector<std::string> args)
{
	// _root가 비어있거나 size값이 1개가 아니면 에러
	if (args.size() != 1 || _root != "")
		throw ServerBlock::ExceptionInvalidArguments();
	// 값 넣어줌
	_root = args[0];
}

void ServerBlock::addAllowedMethods(std::vector<std::string> args)
{
	if (args.empty())
		throw ServerBlock::ExceptionInvalidArguments();
	_allowed_methods.clear();
	for (std::vector<std::string>::iterator i = args.begin(); i != args.end(); i++)
	{
		// 중복 값 들어올 시 에러 -> 필요 없다고 판단되면 지워도 됨
		if (_allowed_methods.count(*i))
			throw ServerBlock::ExceptionInvalidArguments();
		_allowed_methods.insert(*i);
	}
}

void ServerBlock::addErrorPage(std::vector<std::string> args)
{
	std::vector<int>	codes;
	std::string			uri = "";
	size_t				len = args.size();

	for (size_t i = 0; i < len; i++)
	{
		if (isDigits(args[i]))
			codes.push_back(atoi(args[i].c_str()));
		else if (codes.empty())
			throw ServerBlock::ExceptionInvalidArguments();
		else if (i == len - 1)
			uri = args[i];
		else
			throw ServerBlock::ExceptionInvalidArguments();
	}
	if (uri == "")
		throw ServerBlock::ExceptionInvalidArguments();
	for (std::vector<int>::iterator i = codes.begin(); i != codes.end(); i++)
		this->_error_page[*i] = uri;
}

void ServerBlock::addCgiPass(std::vector<std::string> args)
{
	if (args.size() != 1)
		throw ServerBlock::ExceptionInvalidArguments();
	this->_cgi_pass = args[0];
}

void ServerBlock::addClientBodyBufferSize(std::vector<std::string> args)
{
	if (args.size() != 1 || !isDigits(args[0]))
		throw ServerBlock::ExceptionInvalidArguments();
	this->_client_body_buffer_size = atoi(args[0].c_str());
}

void	ServerBlock::addAutoIndex(std::vector<std::string> args)
{
	if (args.size() != 1)
		throw ServerBlock::ExceptionInvalidArguments();
	if (args[0] == "on")
		this->_auto_index = true;
	else if (args[0] == "off")
		this->_auto_index = false;
	else
		throw ServerBlock::ExceptionInvalidArguments();
}

// serverBlock 파싱
int	 ServerBlock::parseServerBlock(unsigned int &index, fileVector &file)
{
	fileVector                  args;
	parseMap::iterator          iter;
	std::string                 directive;

	// arg를 매개 변수로 사용하여 지시어에 해당하는 함수를 호출합니다.
	//serverParsingMap을 돌면서 관련 명령어가 있는 지 파싱
	for ( ; index < file.size() && file[index] != "}" ; index++) {
		// serverParsingMap에 file[index]에 해당하는 것이 없을 경우
		if ((iter = ServerBlock::serverParsingMap.find(file[index])) == ServerBlock::serverParsingMap.end()) {
			
			// 1. 만약 location이라면?
			if (file[index] == "location") {
				ServerBlock	location;
				std::string	locationName;

				// directive가 빈 문자열이 아니라면?
				if (directive != "") {
					// 해당 명령어 함수 동작시키고 arg, directive 초기화
					(*ServerBlock::serverParsingMap[directive])(args);
					args.clear();
					directive = "";
				}
				// 다음 단위가 '{' 또는 '}' 라면 리턴 0 (처음 반복문을 돌 때는 locationName자리임.)
				index++;
				if (file[index] == "{" || file[index] == "}")
					return 0;
				// 아니라면 다음 인덱스는 locationName임.
				locationName = file[index];
				// locationName 다음에 있는 '{' 인덱스부터 parseLocation으로 보내 파싱해줌 -> 잘못되면 리턴 0
				index++;
				if (!location.parseLocation(index, file))
					return 0;
				// 파싱한 값 -> _location에 넣어줌
				_location[locationName] = location;
				// 다음 index값이 "}"라면 조건문 처음으로
				if (file[index] == "}")
					continue ;
			}

			// location이 아니고 directive가 빈 문자열이라면(없다면) 해당 값이 "}"인지 확인 후에 1 또는 0 리턴
			else if (!directive.compare(""))
				return file[index] == "}" ? 1 : 0;
			
			// 그 외 -> 해당 file[index]를 args에 push_back해줌.
			else
				args.push_back(file[index]);
		}
		// 해당하는 명령어가 serverParsingMap에 있을 경우
		else
		{
			// directive에 값이 있을 때 실행함
			if (directive != "") {
				(*ServerBlock::serverParsingMap[directive])(args);
				args.clear();
			}
			// directive에 해당 명령어를 넣음.
			directive = iter->first;
		}
	}
	// directive가 있으면 해당 명령어 실행
	if (directive != "")
		(*ServerBlock::serverParsingMap[directive])(args);
	//  해당 서버 디렉토리스가 파싱이 끝났다면('}'을 만났다면)
	if (!file[index].compare("}")) {
		// 혹시나 빈 값 defaultServer conf값으로 채워줌
		ServerBlock::_defaultServer.passMembers(*this);
		// location도 돌면서 값 채워줌
		for (std::map<std::string, ServerBlock>::iterator i = _location.begin() ; i != _location.end(); i++)
			passMembers(i->second);
		return 1;
	}
	return 0;
};

// Location 파싱
int     ServerBlock::parseLocation(unsigned int &index, fileVector &file) {
	fileVector                  args;
	parseMap::iterator          iter;
	std::string                 directive = "";

	if (file[index++] != "{")
		return 0;
	//	calling the function that corresponds to a directive with its args as parameters
	for ( ; index < file.size() && file[index] != "}" ; index++) {
		// locationParsingMap에 해당 명령어 없을 경우
		if ((iter = ServerBlock::locationParsingMap.find(file[index])) == ServerBlock::locationParsingMap.end()) {
			// 해당 명령어가 location일 경우
			if (file[index] == "location") {
				ServerBlock	location;
				std::string		locationName;

				if (directive != "") {
					(*ServerBlock::locationParsingMap[directive])(args);
					args.clear();
					directive = "";
				}
				index++;
				if (file[index] == "{" || file[index] == "}")
					return 0;
				locationName = file[index];
				index++;
				if (!location.parseLocation(index, file))
					return 0;
				_location[locationName] = location;
				if (file[index] == "}")
					continue ;
			}
			// location 아니고 directive 빈칸일 경우 -> 끝났거나 에러 1 또는 0 리턴
			else if (directive == "")
				return file[index] == "}" ? 1 : 0;
			// location 아니고 directive 있을 경우 -> arg에 넣어주기
			else
				args.push_back(file[index]);
		}
		// locationParsingMap애 해당 명령어 있을 경우
		else
		{
			// directive이 있으면 실행
			if (directive != "") {
				(*ServerBlock::locationParsingMap[directive])(args);
				args.clear();
			}
			// directive없으면 해당 명령어 넣기
			directive = iter->first;
		}
	}
	if (directive != "")
		(*ServerBlock::locationParsingMap[directive])(args);
	//  set up default values if they were not set by the config file
	if (!file[index].compare("}"))
		return 1;
	return 0;
}