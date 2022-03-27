#include "Request.hpp"

Request::Request() : 
	_method (""), _target_path(""), _query(""), _version(""), _body(""), _status_code(200)
{
	initHeaders();
	initValidMethod();
}

Request::Request(std::string request_value) :
	_method (""), _target_path(""), _query(""), _version(""), _body(""), _status_code(200)
{
	initHeaders();
	initValidMethod();
	parseProcess(request_value);
}

Request::~Request() {}

Request::Request(const Request& _Request)
{
	*this = _Request;
}

Request&	Request::operator=(const Request& _Request)
{
	_method = _Request.getMethod();
	_target_path = _Request.getTargetPath();
	_query = _Request.getQuery();
	_version = _Request.getVersion();
	_headers = _Request.getHeaders();
	_body = _Request.getBody();
	_status_code = _Request.getStatus();

	return *this;
}

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
	std::cout << "--------- start line parsing --------" << std::endl;
	std::cout << _method << std::endl;
	std::cout << _target_path << std::endl;
	std::cout << _query << std::endl;
	std::cout << _version << std::endl;

	//헤더 파싱(에러, \r, 빈문자열시 break)
	while ((line = readLine(request_value, i)) != "\r" && line != "" && _status_code != 400)
	{
		// key 읽어줌
		key = readKey(line);
		// value 읽어줌
		value = readValue(line);
		// key값에 해당하는 값이 있으면, 값 넣어주기
		if (_headers.count(key))
				_headers[key] = value;
	}

	// headers 파싱 확인
	std::cout << "--------- header parsing --------" << std::endl;
	std::cout << _headers["Host"] << std::endl;
	std::cout << _headers["User-Agent"] << std::endl;
	std::cout << _headers["Transfer-Encoding"] << std::endl;
	std::cout << _headers["Content-Type"] << std::endl;
	std::cout << _headers["Accept-Encoding"] << std::endl;
	std::cout << _headers["dongslee"] << std::endl;


	// 헤더 읽은 후 이후 값 모두 넘기고 \r\n 제거 후 _body에 값 추가
	if ((i = request_value.find_first_not_of("\r \n", i)) != std::string::npos)
		parseBody(request_value.substr(i, std::string::npos));

	std::cout << "--------- body parsing --------" << std::endl;
	std::cout << _body << std::endl;
}

// 메소드 파싱 함수
void	Request::parseStartLine(const std::string& str)
{
	size_t	i(0);
	
	if (_status_code == 200)
		parseMethod(str, i);
	if (_status_code == 200)
		checkMethod();
	if (_status_code == 200)
		parsePath(str, i);
	if (_status_code == 200)
		parseQuery();
	if (_status_code == 200)
		parseVersion(str, i);
}

// mathod 파싱
void	Request::parseMethod(const std::string& line, size_t &i)
{
	// line에서 띄어쓰기 위치 찾고 띄어쓰기 없으면 400 에러(버전이 없다는 뜻이니까)
	if ((i = line.find_first_of(' ')) == std::string::npos)
	{
		_status_code = 400;
		std::cerr << "Request ERR:: No PATH / HTTP version" << std::endl;
		return ;
	}
	// 띄어쓰기 있으면 _method 할당
	_method.assign(line, 0, i);
}

// path 파싱
void	Request::parsePath(const std::string& line, size_t &i)
{
	size_t	start;
	// i가 띄어쓰기가 아니면 PATH나 HTTP 버전 없는거니까 에러, 띄어쓰기 여러개일 경우 제거
	if ((start = line.find_first_not_of(' ', i)) == std::string::npos)
	{
		_status_code = 400;
		std::cerr << "Request ERR:: No PATH / HTTP version" << std::endl;
		return ;
	}
	// 그 뒤에 띄어쓰기 없으면 HTTP 버전 없으니까 에러
	if ((i = line.find_first_of(' ', start)) == std::string::npos)
	{
		_status_code = 400;
		std::cerr << "Request ERR:: No HTTP version" << std::endl;
		return ;
	}
	// 띄어쓰기 있으면 _path 할당
	_target_path.assign(line, start, i - start);
}

// path에서 쿼리 찾아 쿼리 변수에 넣고, path에서 쿼리 제거 
void				Request::parseQuery()
{
	size_t		i;
	
	//_path에서 ?를 찾음
	i = _target_path.find_first_of('?');
	// 찾으면
	if (i != std::string::npos)
	{
		// 이후 값 모두 _query에 할당
		_query.assign(_target_path, i + 1, std::string::npos);
		// _target_path에서 쿼리 제거
		_target_path = _target_path.substr(0, i);
	}
}

void		Request::parseVersion(const std::string& line, size_t &i)
{
	// 띄어쓰기 제거 후 뒤에 아무것도 없으면 에러
	if ((i = line.find_first_not_of(' ', i)) == std::string::npos)
	{
		_status_code = 400;
		std::cerr << "Request ERR:: No HTTP version" << std::endl;
		return ;
	}
	// HTTP/ 일일이 확인 후, 이후 3자리 값 _version에 할당
	if (line[i] == 'H' && line[i + 1] == 'T' && line[i + 2] == 'T' &&
			line[i + 3] == 'P' && line[i + 4] == '/')
		_version.assign(line, i + 5, 3);
	// _version이 1.0이나 1.1아니면 에러
	if (_version != "1.0" && _version != "1.1")
	{
		_status_code = 400;
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
	_status_code = 400;
}

// body 할당
void	Request::parseBody(const std::string& str)
{
	// 엔터
	char	strip[] = {'\n', '\r'};

	// body에 str 할당
		_body.assign(str);
		// \n \r 2회 제거(1칸 빈칸 필요하므로 -> 사실은 2칸이 아니면 에러 줘야함)
		for (int i = 0; i < 4; i++)
			if (_body.size() > 0 && _body[_body.size() - 1] == strip[i % 2])
				pop(_body);
			else
				break ;
}


//getter
const std::string&	Request::getMethod() const
{
	return this->_method;
}

const std::string&	Request::getTargetPath() const
{
	return this->_target_path;
}

const std::string&	Request::getQuery() const
{
	return this->_query;
}

const std::string&	Request::getVersion() const
{
	return this->_version;
}

const std::map<std::string, std::string>&	Request::getHeaders() const
{
	return this->_headers;
}

const std::string&	Request::getBody() const
{
	return this->_body;
}

const int	Request::getStatus() const
{
	return this->_status_code;
}