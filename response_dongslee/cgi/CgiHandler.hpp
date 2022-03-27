#ifndef CGI_HANDLER_HPP

# define CGI_HANDLER_HPP

# include "../Webserver.hpp"

class CgiHandler {
	private:
		std::map<std::string, std::string>	_env;
		std::string							_body;
	public:
		CgiHandler(void);
		// CgiHandler(Request &request, RequestConfig &config); // sets up env according to the request


		CgiHandler(CgiHandler const &src);
		~CgiHandler(void);
		CgiHandler   	&operator=(CgiHandler const &src);

		char								**_getEnvAsCstrArray() const;
		// void								_initEnv(Request &request, RequestConfig &config);
		void								_initEnv(void);

		std::string		executeCgi(const std::string &scriptName);	// executes cgi and returns body

};

#endif
