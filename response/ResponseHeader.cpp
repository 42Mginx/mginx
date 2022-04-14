#include "ResponseHeader.hpp"
// Member functions

std::string ResponseHeader::getHeader(size_t size, const std::string &path,
                                      const std::string &redirect,
                                      int statusCode, std::string type,
                                      const std::string &contentLocation) {
    std::string header;
    initValues();
    setValues(size, path, redirect, statusCode, type, contentLocation);
    header = "HTTP/1.1 " + toString(statusCode) + " " +
             getStatusMessage(statusCode) + "\r\n";
    header += writeHeader();
    std::cout << header << std::endl;
    return (header);
}

std::string ResponseHeader::notAllowedMethod(GetConf &getConf, int statusCode) {
    std::string header;
    initValues();
    setValues(0, getConf.getContentLocation(), "", statusCode, "",
              getConf.getContentLocation());
    setAllow(getConf.getAllowedMethods());

    if (statusCode == 405)
        header = "HTTP/1.1 405 Method Not Allowed\r\n";
    else if (statusCode == 413)
        header = "HTTP/1.1 413 Payload Too Large\r\n";
    header += writeHeader();

    return (header);
}

std::string ResponseHeader::writeHeader(void) {
    std::string header = "";

    if (_allow != "")
        header += "Allow: " + _allow + "\r\n";
    if (_contentLength != "")
        header += "Content-Length: " + _contentLength + "\r\n";
    if (_contentLocation != "")
        header += "Content-Location: " + _contentLocation + "\r\n";
    if (_contentType != "")
        header += "Content-Type: " + _contentType + "\r\n";
    if (_date != "")
        header += "Date: " + _date + "\r\n";
    if (_lastModified != "")
        header += "Last-Modified: " + _lastModified + "\r\n";
    if (_location != "")
        header += "Location: " + _location + "\r\n";
    if (_server != "")
        header += "Server: " + _server + "\r\n";
    return (header);
}

void ResponseHeader::setValues(size_t size, const std::string &path,
                               const std::string &redirect, int statusCode,
                               std::string type,
                               const std::string &contentLocation) {
    setContentLength(size);
    setContentLocation(contentLocation);
    setContentType(type, path);
    setDate();
    setLastModified(path);
    setServer();
    setTransferEncoding();
    setAllow();
    setLocation(statusCode, redirect);
}

void ResponseHeader::initValues(void) {

    _contentLength = "";
    _contentLocation = "";
    _contentType = "";
    _date = "";
    _lastModified = "";
    _server = "";
    _transferEncoding = "";
    _location = "";
    initErrorMap();
}

std::string ResponseHeader::getStatusMessage(int code) {
    if (_errors.find(code) != _errors.end())
        return _errors[code];
    return ("Unknown Code");
}

void ResponseHeader::initErrorMap() {
    _errors[100] = "Continue";
    _errors[200] = "OK";
    _errors[201] = "Created";
    _errors[204] = "No Content";
    _errors[301] = "Moved Permanently";
    _errors[400] = "Bad Request";
    _errors[403] = "Forbidden";
    _errors[404] = "Not Found";
    _errors[405] = "Method Not Allowed";
    _errors[413] = "Payload Too Large";
    _errors[500] = "Internal Server Error";
}

//############  Setter  ##############
void ResponseHeader::setAllow(std::set<std::string> methods) {
    std::set<std::string>::iterator it = methods.begin();

    while (it != methods.end()) {
        _allow += *(it++);

        if (it != methods.end())
            _allow += ", ";
    }
}

void ResponseHeader::setContentLength(size_t size) {
    _contentLength = toString(size);
}

void ResponseHeader::setContentLocation(const std::string &path) {
    _contentLocation = path;
}

void ResponseHeader::setAllow(const std::string &allow) { _allow = allow; }

void ResponseHeader::setContentType(std::string type, std::string path) {
    if (type != "") {
        _contentType = type;
        return;
    }
    type = path.substr(path.rfind(".") + 1, path.size() - path.rfind("."));
    if (type == "html")
        _contentType = "text/html";
    else if (type == "css")
        _contentType = "text/css";
    else if (type == "js")
        _contentType = "text/javascript";
    else if (type == "jpeg" || type == "jpg")
        _contentType = "image/jpeg";
    else if (type == "png")
        _contentType = "image/png";
    else if (type == "bmp")
        _contentType = "image/bmp";
    else
        _contentType = "text/plain";
}

void ResponseHeader::setDate(void) {
    char buffer[100];
    struct timeval tv;
    struct tm *tm;

    gettimeofday(&tv, NULL);
    tm = gmtime(&tv.tv_sec);
    strftime(buffer, 100, "%a, %d %b %Y %H:%M:%S GMT", tm);
    _date = std::string(buffer);
}

void ResponseHeader::setLastModified(const std::string &path) {
    char buffer[100];
    struct stat stats;
    struct tm *tm;

    if (stat(path.c_str(), &stats) == 0) {
        tm = gmtime(&stats.st_mtime);
        strftime(buffer, 100, "%a, %d %b %Y %H:%M:%S GMT", tm);
        _lastModified = std::string(buffer);
    }
}

void ResponseHeader::setLocation(int code, const std::string &redirect) {
    if (code == 201 || code / 100 == 3) {
        _location = redirect;
    }
}

// utils
std::string ResponseHeader::toString(size_t n) {
    std::stringstream tmp;
    tmp << n;
    return tmp.str();
}

void ResponseHeader::setServer(void) { _server = "Mginx/1.0.0"; }

void ResponseHeader::setTransferEncoding(void) {
    _transferEncoding = "identity"; // 하나밖에 없는건가?
}

// Overloaders

ResponseHeader &ResponseHeader::operator=(const ResponseHeader &src) {
    (void)src;
    return (*this);
}

// Constructors and destructors

ResponseHeader::ResponseHeader(void) { initValues(); }

ResponseHeader::ResponseHeader(const ResponseHeader &src) { (void)src; }

ResponseHeader::~ResponseHeader(void) {}
