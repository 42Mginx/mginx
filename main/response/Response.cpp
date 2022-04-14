#include "Response.hpp"

void			Response::responseSet(Request &request,GetConf &getconf)//(request, getconf ) //받기
{
	_status_code = request.getStatus();
	_request_method = request.getMethod();
	_error_map = getconf.getErrorPage();
	_target_path = getconf.getTargetPath();
	_auto_index = getconf.getAutoIndex();
	//host
	//port


	//405 413 예외처리
	checkClientError(request, getconf);
}

void			Response::run(Request &request,GetConf &getconf)//(request, getconf ) //받기
{
	//Response valule setting

	clock_t start, finish;
    double duration;

	    start = clock();

	responseSet(request, getconf);
		std::string filename("response.txt");
		std::ofstream file_out;
		file_out.open(filename, std::ios_base::app);
		file_out<<"Getconf.getclientbodybuffersize : "<<getconf.getClientBodyBufferSize()<<std::endl;
		file_out<<"request.getbody.size            : "<<request.getBody().size()<<std::endl;
		file_out.close();

		finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    file_out.open(filename, std::ios_base::app);
    file_out<<"Time Stamp[responseSet] : "<<duration<<std::endl;
    file_out.close();

	if(_status_code == 405 || _status_code == 413)
	{
		ResponseHeader header;
		_response = header.notAllowedMethod(getconf, _status_code) + "\r\n";
		return ;
	}
	//Run Method


	if(_request_method == "GET")
		getMethod(request,getconf);
	else if(_request_method == "HEAD")
		headMethod(request,getconf);
	else if(_request_method == "POST")
		postMethod(request,getconf);
	else if(_request_method == "PUT")
		putMethod(request,getconf);
	else if(_request_method == "DELETE")
		deleteMethod(request,getconf);
	else
		std::cout<<"Error : This method is not exist";

		finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;

    file_out.open(filename, std::ios_base::app);
    file_out<<"Time Stamp[runMethod] : "<<duration<<std::endl;
    file_out.close();
}

void	Response::getMethod(Request &request,GetConf &getconf)
{
	ResponseHeader header;

	std::cout<<"@@location 출력1: "<<getconf.getRedirect()<<std::endl;
	if (getconf.getRedirect() != "")
	{
		_status_code = 301;
		_location = getconf.getRedirect();
		std::cout<<"@@location 출력:"<<_location<<std::endl;
	}
	else if (getconf.getCgiPass() != "")
		setCgiResult(request, getconf);
	else if(_status_code == 200)
		_status_code = readContent(); //_body 만 작성된 상태
	else
		_response = this->readHtml(_error_map[_status_code]);
	if (_status_code == 500) //예측하지 못한 서버에러
		_response = this->readHtml(_error_map[_status_code]);
	_response = header.getHeader(_response.size(), _target_path, _location, _status_code, _type, getconf.getContentLocation()) + "\r\n" + _response;
}


//head METHOD
void			Response::headMethod(Request &request,GetConf &getconf)
{
	ResponseHeader	header;

	_status_code = readContent();
	_response = header.getHeader(_response.size(), _target_path,_location, _status_code, _type, getconf.getContentLocation())  + "\r\n" + _response;
}

//post METHOD
void			Response::postMethod(Request &request,GetConf &getconf)
{

	clock_t start, finish;
    double duration;
	std::string filename("response.txt");
	std::ofstream file_out;

	start = clock();

	ResponseHeader	header;

	finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    file_out.open(filename, std::ios_base::app);
    file_out<<"Time Stamp[ResponseHeader] : "<<duration<<std::endl;
    file_out.close();

	start = clock();

	if (getconf.getCgiPass() != "")
		setCgiResult(request, getconf);
	else
	{
		_status_code = 204;
		_response = "";
	}
	finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    file_out.open(filename, std::ios_base::app);
    file_out<<"Time Stamp[GetCgiPass] : "<<duration<<std::endl;
    file_out.close();

	if (_status_code == 500)
		_response = this->readHtml(_error_map[_status_code]);

	start = clock();

	_response = header.getHeader(_response.size(), _target_path,_location, _status_code, _type, getconf.getContentLocation())+ "\r\n" + _response;

	finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    file_out.open(filename, std::ios_base::app);
    file_out<<"Time Stamp[getHeader] : "<<duration<<std::endl;
    file_out.close();
}

//put METHOD
void			Response::putMethod(Request &request,GetConf &getconf)
{

	ResponseHeader	header;
	std::string		content;

	content = request.getBody();
	_response = ""; //_response 초기화
	_status_code = writeContent(content);
	if (_status_code != 201 && _status_code != 204)
		_response = this->readHtml(_error_map[_status_code]);
	_response = header.getHeader(_response.size(), _target_path,_location, _status_code, _type, getconf.getContentLocation()) + "\r\n" + _response;

}

void			Response::deleteMethod(Request &request,GetConf &getconf)
{
	ResponseHeader	header;
	_response = "";
	if (pathIsFile(_target_path))
	{
		if (remove(_target_path.c_str()) == 0)
			_status_code = 204;
		else
			_status_code = 403;
	}
	else
		_status_code = 404;
	if (_status_code == 403 || _status_code == 404)
		_response = this->readHtml(_error_map[_status_code]);
	_response = header.getHeader(_response.size(), _target_path,_location, _status_code, _type, getconf.getContentLocation()) + "\r\n" + _response;
}


//utils
//파일읽기
int				Response::readContent(void)
{
	std::ifstream		file;
	std::stringstream	buffer;

	_response = "";//response초기화
	if (pathIsFile(_target_path)) //path에 파일이 있는지 확인하기 1이면 파일, 나머지는 0, [path = root + target_target_path]
	{
		file.open(_target_path.c_str(), std::ifstream::in); //파일열기
		if (file.is_open() == false)
		{
			_response = this->readHtml(_error_map[403]); //403읽고 반납, 작동중인 서버에 클라이언트의 요청이 도달했으나, 서버가 클라이언트의 접근을 거부할 때 반환하는 HTTP 응답 코드이자 오류 코드이다. 이 에러는 서버 자체 또는 서버에 있는 파일에 접근할 권한이 없을 경우에 발생한다.
			return (403);
		}
		buffer << file.rdbuf(); //현재 읽은 파일을 버퍼에 넣음
		_response = buffer.str(); //_buffer에 있는걸 response에 넣음
		file.close(); //파일 닫기
	}
	else if (_auto_index) // 파일이 없으면 + 오토인덱스가 켜져있으면
	{
		std::cout<<"autoindex test exec"<<std::endl;
		int tmpHost = 0;
		int tmpPort = 80;
		// buffer << AutoIndexGenerator::getPage(_tatgetPath.c_str(), to_string(_hostPort.host), _hostPort.port);
		buffer << AutoIndexGenerator::getPage(_target_path.c_str(), to_string(tmpHost), tmpPort);//HOST, PORT  정리되면 다시 넣어야함
		_response = buffer.str();
		_type = "text/html";
	}
	else
	{
		_response = this->readHtml(_error_map[404]); //404readhtml 넣기
		return (404);
	}
	return (200);
}

int				Response::writeContent(std::string content)
{
	std::ofstream	file;

	if (pathIsFile(_target_path))
	{
		file.open(_target_path.c_str());
		file << content;
		file.close();
		return (204); //흔히 204를 반환하는 경우는 PUT 요청에 대한 응답으로, 사용자에게 보여지는 페이지를 바꾸지 않고 리소스를 업데이트할 때 쓰입니다. 있는 파일에다가 쓸ㅇㅜ
	}
	else
	{
		file.open(_target_path.c_str(), std::ofstream::out | std::ofstream::trunc);
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
void			Response::checkClientError(Request &request,GetConf &getconf)
{
	if(getconf.getAllowedMethods().find(request.getMethod()) == getconf.getAllowedMethods().end())
		_status_code = 405;
	else if (getconf.getClientBodyBufferSize() < request.getBody().size())
		_status_code = 413;
}

//getter
std::string&		Response::getResponse(void)
{
	return (_response);
}

// Overloaders

Response & Response::operator=(const Response & src)
{
	_response = src._response;
	_target_path = src._target_path;
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


void			Response::setCgiResult(Request &request,GetConf &getconf)
{
		CgiHandler	cgi(request, getconf);
		size_t		i = 0;
		size_t		j = _response.size() - 2;

		clock_t start, finish;
   		 double duration;
					std::string filename("response.txt");
					std::ofstream file_out;

	    start = clock();



		_response = cgi.executeCgi(getconf.getCgiPass()); //cig결과값 _response에 넣기

			finish = clock();
 	   duration = (double)(finish - start) / CLOCKS_PER_SEC;
 	   file_out.open(filename, std::ios_base::app);
 	   file_out<<"Time Stamp[executeCGI] : "<<duration<<std::endl;
  	  file_out.close();

  start = clock();
		while (_response.find("\r\n\r\n", i) != std::string::npos || _response.find("\r\n", i) == i)
		{
			std::string	str = _response.substr(i, _response.find("\r\n", i) - i);
			if (str.find("Status: ") == 0)
				_status_code = std::atoi(str.substr(8, 3).c_str());
			else if (str.find("Content-Type: ") == 0)
				_type = str.substr(14, str.size());
			i += str.size() + 2;
		}


		while (_response.find("\r\n", j) == j)
			j -= 2;


			finish = clock();
 	   duration = (double)(finish - start) / CLOCKS_PER_SEC;
 	   file_out.open(filename, std::ios_base::app);
 	   file_out<<"Time Stamp[find rn] : "<<duration<<std::endl;
  	  file_out.close();

		  start = clock();


		_response = _response.substr(i, j - i);

			finish = clock();
 	   duration = (double)(finish - start) / CLOCKS_PER_SEC;
 	   file_out.open(filename, std::ios_base::app);
 	   file_out<<"Time Stamp[response.substr] : "<<duration<<std::endl;
  	  file_out.close();
}


void			Response::initResponse(void)
{
	_response = "";
}
