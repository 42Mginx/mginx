#include "CgiHandler.hpp"

CgiHandler	&CgiHandler::operator=(CgiHandler const &src) {
	if (this != &src) {
		this->_body = src._body;
		this->_env = src._env;
	}
	return *this;
}


void		CgiHandler::_initEnv(Request &request, GetConf &getConf)
{

	std::map<std::string, std::string>	headers = request.getHeaders(); //request에서 '요청헤더' 가져옴

	// if (headers.find("Auth-Scheme") != headers.end() && headers["Auth-Scheme"] != "")
		// this->_env["AUTH_TYPE"] = headers["Authorization"];

	// this->_env["REDIRECT_STATUS"] = "200"; //Security needed to execute php-cgi
	// this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->_env["SCRIPT_NAME"] = getConf.getContentLocation(); //HTTP 요청의 첫 번째 라인에 있는 조회 문자열까지의 URL.
	// this->_env["SCRIPT_FILENAME"] = config.getPath();
	this->_env["REQUEST_METHOD"] = request.getMethod();
	// this->_env["CONTENT_LENGTH"] = to_string(this->_body.length());
	// this->_env["CONTENT_TYPE"] = headers["Content-Type"];
	this->_env["PATH_INFO"] = request.getTargetPath(); //might need some change, using config path/contentLocation
	this->_env["PATH_TRANSLATED"] = request.getTargetPath(); //might need some change, using config path/contentLocation //진짜경로
	// this->_env["QUERY_STRING"] = request.getQuery();
	// this->_env["REMOTE_ADDRr"] = to_string(config.getHostPort().host);
	// this->_env["REMOTE_IDENT"] = headers["Authorization"];
	// this->_env["REMOTE_USER"] = headers["Authorization"];
	// this->_env["REQUEST_URI"] = request.getPath() + request.getQuery(); //현재 페이지 주소에서 도메인을 제외한 값.
	// if (headers.find("Hostname") != headers.end())
		// this->_env["SERVER_NAME"] = headers["Hostname"];
	// else
		// this->_env["SERVER_NAME"] = this->_env["REMOTEaddr"];

	// this->_env["SERVER_PORT"] = to_string(config.getHostPort().port);
	this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	// this->_env["SERVER_SOFTWARE"] = "Weebserv/1.0";

	this->_env.insert(getConf.getCgiParam().begin(), getConf.getCgiParam().end());
}


char					**CgiHandler::_getEnvAsCstrArray() const {
	char	**env = new char*[this->_env.size() + 1];
	int	j = 0;

	for (std::map<std::string, std::string>::const_iterator it = this->_env.begin(); it != this->_env.end(); it++) {
		std::string	element = it->first + "=" + it->second;
		env[j] = new char[element.size() + 1];
		env[j] = strcpy(env[j], (const char*)element.c_str());
		j++;
	}
	env[j] = NULL;
	return env;
}

std::string		CgiHandler::executeCgi(const std::string& scriptName) {
	pid_t		pid;
	int			tmpStdin;
	int			tmpStdout;
	char		**env;
	std::string	cgiBody;

	try {
		env = this->_getEnvAsCstrArray();
		/* CGI ENV 출력 TEST
		std::cout<<"env cout"<<std::endl;
		for(int i = 0;env[i] != NULL;i++)
			std::cout<<env[i]<<std::endl;
		*/
	}
	catch (std::bad_alloc &e) {
		std::cerr << e.what()  << std::endl;
	}

	tmpStdin = dup(STDIN_FILENO);
	tmpStdout = dup(STDOUT_FILENO);

	FILE	*fIn = tmpfile();
	FILE	*fOut = tmpfile();
	long	fdIn = fileno(fIn);
	long	fdOut = fileno(fOut);
	int		ret = 1;

	write(fdIn, _body.c_str(), _body.size());
	lseek(fdIn, 0, SEEK_SET);

	pid = fork();

	if (pid == -1)
	{
		std::cerr <<  "Fork crashed." << std::endl;
		return ("Status: 500\r\n\r\n");
	}
	else if (pid == 0)
	{
		char * const * nll = NULL;

		dup2(fdIn, STDIN_FILENO);
		dup2(fdOut, STDOUT_FILENO);
		execve(scriptName.c_str(), nll, env);
		// execve(scriptName2.c_str(), nll, env);
		std::cerr << "CGI Execve crashed." << std::endl;
		write(STDOUT_FILENO, "Status: 500\r\n\r\n", 15);
	}
	else
	{
		char	buffer[CGI_BUFSIZE] = {0};

		waitpid(pid, NULL, 0); //두번째 인자 정리
		lseek(fdOut, 0, SEEK_SET);

		ret = 1;
		while (ret > 0)
		{
			memset(buffer, 0, CGI_BUFSIZE);
			ret = read(fdOut, buffer, CGI_BUFSIZE - 1);
			cgiBody += buffer;
		}
	}

	dup2(tmpStdin, STDIN_FILENO);
	dup2(tmpStdout, STDOUT_FILENO);
	fclose(fIn);
	fclose(fOut);
	close(fdIn);
	close(fdOut);
	close(tmpStdin);
	close(tmpStdout);

	for (size_t i = 0; env[i]; i++)
		delete[] env[i];
	delete[] env;

	return (cgiBody);
}

//

CgiHandler::CgiHandler(Request &request, GetConf &getConf):
_body(request.getBody())
{
	this->_initEnv(request, getConf);
}

CgiHandler::CgiHandler(CgiHandler const &src) {
	if (this != &src) {
		this->_body = src._body;
		this->_env = src._env;
	}
	return ;
}

CgiHandler::~CgiHandler(void) {
	return ;
}
