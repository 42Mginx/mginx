#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Webserver.hpp"
# include "ResponseHeader.hpp"
# include "GetConf.hpp"
# include "CgiHandler.hpp"
# include "AutoIndexGenerator.hpp"
# include "Request.hpp"


// typedef struct s_listen {
//     unsigned int host;
//     int port;
// } t_listen;

class Response {
private:

	std::string					_response;
	std::string					_target_path;
	int							_status_code;
	std::string					_type;
	std::string					_request_method;
	bool						_auto_index;
	t_listen					_hostPort;
	std::map<int, std::string>	_error_map;


public:
	Response(void);
	Response(const Response & src);
	~Response(void);

	Response & operator=(const Response & src);

	std::string		getResponse(void);

	//response setting
	void	responseSet(Request &request,GetConf &getconf);

	//resposne 실행
	void	run(Request &request,GetConf &getconf);

	//reqeuest method 따라서 실행
	void	getMethod(Request &request, GetConf &getconf);
	void	headMethod(Request &request,GetConf &getconf);
	void	postMethod(Request &request,GetConf &getconf);
	void	putMethod(Request &request,GetConf &getconf);
	void	deleteMethod(Request &request,GetConf &getconf);

	//utils
	int				readContent(void);
	int				writeContent(std::string content);
	std::string		readHtml(const std::string& path);
	int				pathIsFile(const std::string& path);
	std::string		to_string(size_t n);
	void			checkClientError(Request &request, GetConf &getconf);
	void			setCgiResult(Request &request,GetConf &getconf);
};



#endif
