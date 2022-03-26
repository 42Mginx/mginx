#ifndef CGI_HANDLER_HPP

# define CGI_HANDLER_HPP

# include "../Webserver.hpp"
# include "../request/Request.hpp"
# include "../response/GetConf.hpp"

# define CGI_BUFSIZE 65536

class CgiHandler {
	private:
		std::map<std::string, std::string>	_env;
		std::string							_body;
	public:
		CgiHandler(void);
		CgiHandler(Request &request, GetConf &getConf); // sets up env according to the request


		CgiHandler(CgiHandler const &src);
		~CgiHandler(void);
		CgiHandler   	&operator=(CgiHandler const &src);

		char								**_getEnvAsCstrArray() const;
		void								_initEnv(Request &request, GetConf &getConf);


		std::string		executeCgi(const std::string &scriptName);	// executes cgi and returns body

};

#endif
