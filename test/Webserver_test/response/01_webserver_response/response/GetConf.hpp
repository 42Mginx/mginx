#ifndef GETCONF_HPP
# define GETCONF_HPP

# include "../Webserver.hpp"
# include "../request/Request.hpp"
# include "../ServerBlock.hpp"

//Request, ServerBlock 인자로 받아오기
class GetConf{
	private:

		std::string							_content_location; // 실제 대상 파일 위치
		std::string							_target_path; // request 요청 위치
		std::map<int, std::string>			_error_page; // 에러페이지 위치
		unsigned long						_client_body_buffer_size; // Conf에서 제한한 Client max body size, defaults to 8 000
		std::map<std::string, std::string>	_cgi_param;
		std::string							_cgi_ass; // CGI 위치
		std::set<std::string>				_allowed_methods; // allowed http METHOD for request
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
		const std::string&							getContentLocation() const;
		const std::string&							getTargetPath() const;
		const std::map<int, std::string>&			getErrorPage() const;
		const unsigned long&						getClientBodyBufferSize() const;
		const std::map<std::string, std::string>&	getCgiParam() const;
		const std::string&							getCgiPass() const;
		const std::string&							getLang() const;
		const std::set<std::string>&				getAllowedMethods() const;
		// const t_listen							&getHostPort() const;
		bool  										getAutoIndex() const;

	//utils
		// std::string							addIndex(Request& request);


};
#endif
