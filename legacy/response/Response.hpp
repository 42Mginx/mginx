#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
#include "ServerBlock.hpp"

class Response {
   public:
    Response();

    void run(Request request, ServerBlock serverblock);
    std::string getResponse();
};

#endif