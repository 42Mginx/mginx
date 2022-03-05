#include "Request.hpp"
#include "Request_util.hpp"

Request::Request() {}

Request::Request(std::string request_value) :
	_method (""), _target_path(""), _query(""), _version(""), _body(""), _status(200)
{
	initHeaders();
	initValidMethod();
	parse(request_value);
}

Request::~Request() {}

void Request::initHeaders()
{
	// 메모리 초기화
	_headers.clear();
	_headers["Accept"] = "";
	_headers["Accept-Encoding"] = "";
	_headers["Connection"] = "Keep-Alive";
	_headers["Content-Length"] = "";
	_headers["Content-Type"] = "";
	_headers["Host"] = "";
	_headers["User-Agent"] = "";
	_headers["Transfer-Encoding"] = "";
}

void	Request::initValidMethod()
{
	_valid_methods.push_back("GET");
	_valid_methods.push_back("POST");
	_valid_methods.push_back("DELETE");
}

void	Request::parseProcess(std::string request_value)
{

	std::string		key;
	std::string		value;
	std::string		line;
	size_t			i(0);

	// 첫째 줄 파싱해서 읽기(method, http 버전)
	parseStartLine(readLine(request_value, i));

	//헤더 파싱(에러, \r, 빈문자열시 break)
	while ((line = readLine(request_value, i)) != "\r" && line != "" && _status != 400)
	{
		// key 읽어줌
		key = readKey(line);
		// value 읽어줌
		value = readValue(line);
		// key값에 해당하는 값ㅇㅣ 있으면, 값 넣어주기
		if (_headers.count(key))
				_headers[key] = value;
	}
	setBody(request_value.substr(i, std::string::npos));
	// path에서 쿼리 찾아 쿼리 변수에 넣고, path에서 쿼리 제거 
	findQuery();
	return _status;
}

// 메소드 파싱 함수
void	Request::parseStartLine(const std::string& str)
{
	size_t	i;
	std::string	line;
	// string에서 ‘\n’ 위치 찾아서 한 줄 잘라서 line에 넣음.
	// 이거 없어도 될 것 같은 게 어짜피 들어오는 str에 \n이 제거 되어서 들어오기 때문에
	// i = npos가 되고, substr이 한 줄 그대로를 읽음. 그럴거면 그냥 str을 쓰면 됨. 
	i = str.find_first_of('\n');
	line = str.substr(0, i);
	// method 파싱
	if (_status == 200)
		parseMethod(line, i);
	// path 파싱
	if (_status == 200)
		parsePath(line, i);
	// Version 파싱
	if (_status == 200)
		parseVersion(line, i);
	// 이상 없으면 checkMethod로 갑시다
	if (_status == 200)
		checkMethod();
}

// mathod 파싱
void	Request::parseMethod(const std::string& line, size_t i)
{
	// line에서 띄어쓰기 위치 찾고 띄어쓰기 없으면 400 에러(버전이 없다는 뜻이니까)
	if ((i = line.find_first_of(' ')) == std::string::npos)
	{
		_status = 400;
		std::cerr << "Request ERR:: No PATH / HTTP version" << std::endl;
		return ;
	}
	// 띄어쓰기 있으면 _method 할당
	_method.assign(line, 0, i);
}

// path 파싱
void	Request::parsePath(const std::string& line, size_t i)
{
	size_t	j;

	// 그 뒤에 띄어쓰기 없으면 HTTP 버전 없는거니까 에러
	if ((i = line.find_first_of(' ', j)) == std::string::npos)
	{
		_status = 400;
		std::cerr << "Request ERR:: No HTTP version" << std::endl;
		return ;
	}
	// 띄어쓰기 있으면 _path 할당
	_target_path.assign(line, j, i - j);
}

void		Request::parseVersion(const std::string& line, size_t i)
{
	// HTTP/ 일일이 확인 후, 이후 3자리 값 _version에 할당
	if (line[i] == 'H' && line[i + 1] == 'T' && line[i + 2] == 'T' &&
			line[i + 3] == 'P' && line[i + 4] == '/')
		_version.assign(line, i + 5, 3);

	// _version이 1.0이나 1.1아니면 에러
	if (_version != "1.0" && _version != "1.1")
	{
		_status = 400;
		std::cerr << "Request ERR:: UNSUPPORTED HTTP VERSION (" << _version << ")" << std::endl;
		return ;
	}
}

void	Request::checkMethod()
{
	// 기존 map에 정리해놓은 _valid_methods안에 우리가 파싱한 method 있는 지 확인 후 리턴
	for (size_t i = 0; i < _valid_methods.size(); i++)
		if (_valid_methods[i] == _method)
			return ;
	// 없는 메소드면 에러띄우고 status 변경
	std::cerr << "Request ERR:: Invalid method requested" << std::endl;
	_status = 400;
}