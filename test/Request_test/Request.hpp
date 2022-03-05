#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "Request_util.hpp"

class Request {
		private:
			std::string 								_method;
			std::string									_target_path;
			// 일단 cgi가 path와 query의 조합으로 이뤄지기 때문에 먼저 cgi를 따로 만들 필요는 없을 듯
			std::string									_query;
			std::string									_version;
			std::map<std::string, std::string>			_headers;
			std::string									_body;
			// std::map<std::string, std::string>			env_for_cgi;
			int											_status;
			std::vector<std::string>					_valid_methods;

		public:
			Request();
			Request(std::string request_value);
			~Request();
			void	initHeaders();
			void	initValidMethod();
			void	parseProcess(std::string request_value);
			void	parseStartLine(const std::string& str);
			void	parseMethod(const std::string& line, size_t i);
			void	parsePath(const std::string& line, size_t i);
			void	parseVersion(const std::string& line, size_t i);
			void	checkMethod();
};

#endif