#ifndef RESPONSEHEADER_HPP
#define RESPONSEHEADER_HPP

#include "AutoIndexGenerator.hpp"
#include "CgiHandler.hpp"
#include "GetConf.hpp"
#include "Request.hpp"
#include "Webserver.hpp"

class ResponseHeader {
  private:
    std::string _allow;
    std::string _contentLength;
    std::string _contentLocation;
    std::string _contentType;
    std::string _date;
    std::string _lastModified;
    std::string _server;
    std::string _transferEncoding;
    std::map<int, std::string> _errors;

    std::string _location;

    void initErrorMap();

  public:
    ResponseHeader(void);
    ResponseHeader(const ResponseHeader &src);
    ~ResponseHeader(void);

    ResponseHeader &operator=(const ResponseHeader &src);

    // Member functions
    std::string getHeader(size_t size, const std::string &path,
                          const std::string &redirect, int statusCode,
                          std::string type, const std::string &contentLocation);
    std::string notAllowedMethod(GetConf &getConf, int statusCode);
    std::string writeHeader(void);
    void setValues(size_t size, const std::string &path,
                   const std::string &redirect, int statusCode,
                   std::string type, const std::string &contentLocation);
    void initValues(void);
    std::string getStatusMessage(int code);

    // Setter functions
    void setAllow(const std::string &allow = "");
    void setAllow(std::set<std::string> methods);
    void setContentLength(size_t size);
    void setContentLocation(const std::string &path);
    void setContentType(std::string type, std::string path);
    void setDate(void);
    void setLastModified(const std::string &path);
    void setLocation(int code, const std::string &redirect);
    void setServer(void);
    void setTransferEncoding(void);

    // utils
    std::string toString(size_t n);
};

#endif
