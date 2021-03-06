#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "RequestUtil.hpp"

class Request {
  private:
    std::string _method;
    std::string _target_path;
    std::string _query;
    std::string _version;
    std::map<std::string, std::string> _headers;
    std::string _body;
    int _status_code;
    std::vector<std::string> _valid_methods;
    std::map<std::string, std::string> _env_for_cgi;

  public:
    Request();
    Request(std::string &request_value);
    ~Request();
    Request(const Request &_Request);
    Request &operator=(const Request &);

    void initHeaders();
    void initBody();
    void initValidMethod();
    std::string formatHeaderForCGI(std::string &key);
    void parseProcess(std::string &request_value);
    void parseStartLine(const std::string &str);
    void parseMethod(const std::string &line, size_t &i);
    void parsePath(const std::string &line, size_t &i);
    void parseVersion(const std::string &line, size_t &i);
    void checkMethod();
    void parseBody(const std::string &str);
    void parseQuery();

    const std::string &getMethod() const;
    const std::string &getTargetPath() const;
    const std::string &getQuery() const;
    const std::string &getVersion() const;
    const std::map<std::string, std::string> &getHeaders() const;
    const std::string &getBody() const;
    const int getStatus() const;
    const std::map<std::string, std::string> &getEnv() const;
};

#endif
