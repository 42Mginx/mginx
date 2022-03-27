#ifndef RESPONSEHEADER_HPP
# define RESPONSEHEADER_HPP

# include "Webserver.hpp"
# include "CgiHandler.hpp"
# include "AutoIndexGenerator.hpp"
# include "Request.hpp"
# include "GetConf.hpp"


class ResponseHeader {
private:
	std::string					_allow;
	std::string					_contentLength;
	std::string					_contentLocation;
	std::string					_contentType;
	std::string					_date;
	std::string					_lastModified;
	std::string					_server;
	std::string					_transferEncoding;
	std::map<int, std::string>	_errors;

	void						initErrorMap();

public:
	ResponseHeader(void);
	ResponseHeader(const ResponseHeader & src);
	~ResponseHeader(void);

	ResponseHeader & operator=(const ResponseHeader & src);

		// Member functions
	std::string		getHeader(size_t size, const std::string& path, int statusCode, std::string type, const std::string& contentLocation);
	// std::string		notAllowed(std::set<std::string> methods, const std::string& path, int code, const std::string& lang);
	std::string		notAllowedMethod(GetConf &getConf, int statusCode);
	std::string		writeHeader(void);
	void			setValues(size_t size, const std::string& path,int statusCode, std::string type, const std::string& contentLocation);
	void			initValues(void);
	std::string		getStatusMessage(int code);

	// Setter functions
	void			setAllow(const std::string& allow = "");
	void			setContentLength(size_t size);
	void			setContentLocation(const std::string& path);
	void			setContentType(std::string type, std::string path);
	void			setDate(void);
	void			setLastModified(const std::string& path);
	void			setLocation(int code, const std::string& redirect);
	void			setServer(void);
	void			setTransferEncoding(void);

	//utils
	std::string		toString(size_t n);

};

#endif
