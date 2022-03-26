#include "Response.hpp"

void			Response::responseSet(Request &request,GetConf &getConf)//(request, getConf ) //받기
{
	_statusCode = request.getStatus();
	_targetPath = request.getTargetPath();
	_errorMap = getConf.getErrorPage();
	_isAutoIndex = getConf.getAutoIndex();
	_requestMethod = request.getMethod();

	//405 413 예외처리
	checkClientError(request, getConf);
}

void			Response::run(Request &request,GetConf &getConf)//(request, getConf ) //받기
{
	//Response valule setting
	responseSet(request, getConf);
	if(_statusCode == 405 || _statusCode == 413)
	{
		ResponseHeader header;
		_response = header.notAllowedMethod(getConf, _statusCode);
		return ;
	}
	//Run Method
	if(_requestMethod == "GET")
		getMethod(request,getConf);
	else if(_requestMethod == "HEAD")
		headMethod(request,getConf);
	else if(_requestMethod == "POST")
		postMethod(request,getConf);
	else if(_requestMethod == "PUT")
		putMethod(request,getConf);
	else if(_requestMethod == "DELETE")
		deleteMethod(request,getConf);
	else
		std::cout<<"Error : This method is not exist";
}

void	Response::getMethod(Request &request,GetConf &getConf)
{
	ResponseHeader header;

	if (getConf.getCgiPass() != "")
	{
		std::cout<<"Cgi_Pass : "<<getConf.getCgiPass()<<std::endl;
		CgiHandler	cgi(request, getConf);
		_response = cgi.executeCgi(getConf.getCgiPass()); //cig결과값 _response에 넣기
	}
	else if(_statusCode == 200)
		_statusCode = readContent(); //_body 만 작성된 상태
	else
		_response = this->readHtml(_errorMap[_statusCode]);
	if (_statusCode == 500) //예측하지 못한 서버에러
		_response = this->readHtml(_errorMap[_statusCode]);
	_response = header.getHeader(_response.size(), _targetPath, _statusCode, _type, getConf.getContentLocation()) + "\r\n" + _response;
}


//head METHOD
void			Response::headMethod(Request &request,GetConf &getConf)
{
	ResponseHeader	header;

	_statusCode = readContent();
	_response = header.getHeader(_response.size(), _targetPath, _statusCode, _type, getConf.getContentLocation() +"\r\n");
}

//post METHOD
void			Response::postMethod(Request &request,GetConf &getConf)
{
	ResponseHeader	header;

	if (getConf.getCgiPass() != "")
	{
		std::cout<<"Cgi_Pass : "<<getConf.getCgiPass()<<std::endl;
		CgiHandler	cgi(request, getConf);
		_response = cgi.executeCgi(getConf.getCgiPass()); //cig결과값 _response에 넣기
	}
	else if(_statusCode == 204)
	{
		_statusCode = 204;
		_response = "";
	}
	if (_statusCode == 500)
		_response = this->readHtml(_errorMap[_statusCode]);
	_response = header.getHeader(_response.size(), _targetPath, _statusCode, _type, getConf.getContentLocation() + "\r\n" + _response);
}

//put METHOD
void			Response::putMethod(Request &request,GetConf &getConf)
{

	ResponseHeader	header;
	std::string		content;

	content = request.getBody();
	_response = ""; //_response 초기화
	_statusCode = writeContent(content);
	if (_statusCode != 201 && _statusCode != 204)
		_response = this->readHtml(_errorMap[_statusCode]);
	_response = header.getHeader(_response.size(), _targetPath, _statusCode, _type, getConf.getContentLocation() + "\r\n" + _response);

}

void			Response::deleteMethod(Request &request,GetConf &getConf)
{
	request.coutRequest();
	getConf.getConfTester();

	ResponseHeader	header;
	_response = "";
	if (pathIsFile(_targetPath))
	{
		if (remove(_targetPath.c_str()) == 0)
			_statusCode = 204;
		else
			_statusCode = 403;
	}
	else
		_statusCode = 404;
	if (_statusCode == 403 || _statusCode == 404)
		_response = this->readHtml(_errorMap[_statusCode]);
	_response = header.getHeader(_response.size(), _targetPath, _statusCode, _type, getConf.getContentLocation() + "\r\n" + _response);
}


//utils
//파일읽기
int				Response::readContent(void)
{
	std::ifstream		file;
	std::stringstream	buffer;

	_response = "";//response초기화
	if (pathIsFile(_targetPath)) //path에 파일이 있는지 확인하기 1이면 파일, 나머지는 0, [path = root + target_targetPath]
	{
		file.open(_targetPath.c_str(), std::ifstream::in); //파일열기
		if (file.is_open() == false)
		{
			_response = this->readHtml(_errorMap[403]); //403읽고 반납, 작동중인 서버에 클라이언트의 요청이 도달했으나, 서버가 클라이언트의 접근을 거부할 때 반환하는 HTTP 응답 코드이자 오류 코드이다. 이 에러는 서버 자체 또는 서버에 있는 파일에 접근할 권한이 없을 경우에 발생한다.
			return (403);
		}
		buffer << file.rdbuf(); //현재 읽은 파일을 버퍼에 넣음
		_response = buffer.str(); //_buffer에 있는걸 response에 넣음
		file.close(); //파일 닫기
	}
	else if (_isAutoIndex) // 파일이 없으면 + 오토인덱스가 켜져있으면
	{
		std::cout<<"autoindex test 실행됨"<<std::endl;
		int tmpHost = 0;
		int tmpPort = 80;
		// buffer << AutoIndexGenerator::getPage(_tatgetPath.c_str(), to_string(_hostPort.host), _hostPort.port);
		buffer << AutoIndexGenerator::getPage(_targetPath.c_str(), to_string(tmpHost), tmpPort);//HOST, PORT  정리되면 다시 넣어야함
		_response = buffer.str();
		_type = "text/html";
	}
	else
	{
		_response = this->readHtml(_errorMap[404]); //404readhtml 넣기
		return (404);
	}
	return (200);
}

int				Response::writeContent(std::string content)
{
	std::ofstream	file;

	if (pathIsFile(_targetPath))
	{
		file.open(_targetPath.c_str());
		file << content;
		file.close();
		return (204); //흔히 204를 반환하는 경우는 PUT 요청에 대한 응답으로, 사용자에게 보여지는 페이지를 바꾸지 않고 리소스를 업데이트할 때 쓰입니다. 있는 파일에다가 쓸ㅇㅜ
	}
	else
	{
		file.open(_targetPath.c_str(), std::ofstream::out | std::ofstream::trunc);
		if (file.is_open() == false)
			return (403);

		file << content;
		file.close();
		return (201);
	}
}


std::string		Response::readHtml(const std::string& path)
{
	std::ofstream		file;
	std::stringstream	buffer;

	if (pathIsFile(path))
	{
		file.open(path.c_str(), std::ifstream::in);
		if (file.is_open() == false)
			return ("<!DOCTYPE html>\n<html><title>404</title><body>There was an error finding your error page</body></html>\n");

		buffer << file.rdbuf();
		file.close();
		_type = "text/html";

		return (buffer.str());
	}
	else
		return ("<!DOCTYPE html>\n<html><title>404</title><body>There was an error finding your error page</body></html>\n");
}

int		Response::pathIsFile(const std::string& path)
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

std::string	Response::to_string(size_t n)
{
	std::stringstream tmp;

	tmp << n;

	return tmp.str();
}

//405 413 예외처리
void			Response::checkClientError(Request &request,GetConf &getConf)
{
	if(getConf.getAllowedMethods().find(request.getMethod()) == getConf.getAllowedMethods().end())
		_statusCode = 405;
	else if (getConf.getClientBodyBufferSize() < request.getBody().size())
		_statusCode = 413;
}

//getter
std::string		Response::getResponse(void)
{
	return (_response);
}

// Overloaders

Response & Response::operator=(const Response & src)
{
	_response = src._response;
	_targetPath = src._targetPath;
	return (*this);
}


// Constructors and destructors

Response::Response(void)
{
}

Response::Response(const Response & src)
{
	*this = src;
}

Response::~Response(void)
{
}
