#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "../Webserver.hpp"

//헤엄이 작성한 리퀘스트 더미

class Request {
	private:
		std::string 								_method;
		std::string									_target_path;
		std::string									_query;
		std::string									_version;
		std::map<std::string, std::string>			_headers;
		std::string									_body;
		int											_status_code;
		std::vector<std::string>					_valid_methods;

	public:
		Request();
		~Request();
		void coutRequest();

		//getter
		const std::string&									getMethod() const;
		const std::string&									getTargetPath() const;
		const std::string&									getQuery() const;
		const std::string&									getVersion() const;
		const std::map<std::string, std::string>&			getHeaders() const;
		const std::string&									getBody() const;
		const int											getStatus() const;


};

# endif
