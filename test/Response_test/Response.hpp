#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Webserver.hpp"
# include "ResponseHeader.hpp"
# include "cgi/CgiHandler.hpp"
# include "autoindex/AutoIndexGenerator.hpp"
# include "request/Request.hpp"
# include "GetConf.hpp"

typedef struct s_listen {
    unsigned int host;
    int port;
} t_listen;

class Response {
private:

	std::string					_response;
	std::string					_targetPath;
	int							_statusCode;
	std::string					_type;
	std::string					_requestMethod;
	bool						_isAutoIndex;
	t_listen					_hostPort;
	std::map<int, std::string>	_errorMap;


public:
	Response(void);
	Response(const Response & src);
	~Response(void);

	Response & operator=(const Response & src);

	std::string		getResponse(void);

	//resposne 실행
	void	run(Request &request,GetConf &getConf);

	//reqeuest method 따라서 실행
	void	getMethod(Request &request, GetConf &getConf);
	void	headMethod(Request &request,GetConf &getConf);
	void	postMethod(Request &request,GetConf &getConf);
	void	putMethod(Request &request,GetConf &getConf);
	void	deleteMethod(Request &request,GetConf &getConf);

	//utils
	int				readContent(void);
	int				writeContent(std::string content);
	std::string		readHtml(const std::string& path);
	int				pathIsFile(const std::string& path);
	std::string					to_string(size_t n);



};

#endif
