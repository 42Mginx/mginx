#ifndef GETCONF_HPP
# define GETCONF_HPP

# include "Webserver.hpp"
# include "./request/Request.hpp"
# include "ServerBlock.hpp"

//Request, ServerBlock 인자로 받아오기
class GetConf{
	private:
		std::string							_contentLocation; // 실제 대상 파일 위치
		std::string							_targetPath; // request 요청 위치
		std::map<int, std::string>			_errorPage; // 에러페이지 위치
		unsigned long						_clientBodyBufferSize; // Conf에서 제한한 Client max body size, defaults to 8 000
		std::map<std::string, std::string>	_cgiParam;
		std::string							_cgiPass; // CGI 위치
		std::set<std::string>				_allowedMethods; // allowed http METHOD for request
		// t_listen							_hostPort;

		//아래 두 멤버는 요청의 끝자리가 '/'일때만 필요, autoindex는 해당파일이 존재하지않을대도 필요
		std::vector<std::string>			_index; // GET요청에 아무것도 들어 오지않앗을 경우 처리
		bool								_autoindex; // Autoindex FLAG

	public:
		GetConf();
		GetConf(Request &request, ServerBlock &serverblock);
		~GetConf();

	//tester
		void 										getConfTester();

	//getter
		const std::string&							GetContentLocation() const;
		const std::string&							GetTargetPath() const;
		const std::map<int, std::string>&			GetErrorPage() const;
		const unsigned long&						GetClientBodyBufferSize() const;
		const std::map<std::string, std::string>&	GetCgiParam() const;
		const std::string&							GetCgiPass() const;
		const std::string&							GetLang() const;
		const std::set<std::string>&				GetAllowedMethods() const;
		// const t_listen							&getHostPort() const;
		bool  										GetAutoIndex() const;

	//utils
		// std::string							addIndex(Request& request);


};
#endif
