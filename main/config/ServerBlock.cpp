#include "./ServerBlock.hpp"

/*------------- 캐노니컬  --------------------------------------------------*/

// INITIALIZING STATIC MEMBERS

ServerBlock ServerBlock::_default_conf = ServerBlock();

ServerBlock::ServerBlock()
    : _root(""), _client_body_buffer_size(0), _auto_index(false),
      _aliasSet(false)
{
    _initDirectivesMap();
};
ServerBlock::~ServerBlock(void) {}
ServerBlock &ServerBlock::operator=(ServerBlock const &src)
{
    if (this != &src)
    {
        _server_name = src._server_name;
        _listen = src._listen;
        _root = src._root;
        _allowed_methods = src._allowed_methods;
        _error_page = src._error_page;
        _client_body_buffer_size = src._client_body_buffer_size;
        _auto_index = src._auto_index;
        _location = src._location;
    }
    return *this;
}

/*------------- 지시어 파싱 맵과 함수들
 * --------------------------------------------------*/

directivesMap ServerBlock::_initDirectivesMap()
{
    directivesMap tmpMap;

    tmpMap["server_name"] = &ServerBlock::addServerName;
    tmpMap["listen"] = &ServerBlock::addListen;
    tmpMap["root"] = &ServerBlock::addRoot;
    tmpMap["error_page"] = &ServerBlock::addErrorPage;
    tmpMap["client_body_buffer_size"] = &ServerBlock::addClientBodyBufferSize;
    tmpMap["cgi_param"] = &ServerBlock::addCgiParam;
    tmpMap["cgi_pass"] = &ServerBlock::addCgiPass;
    tmpMap["autoindex"] = &ServerBlock::addAutoIndex;
    tmpMap["index"] = &ServerBlock::addIndex;
    tmpMap["allow_methods"] = &ServerBlock::addAllowedMethods;

    return (tmpMap);
}

directivesMap ServerBlock::directivesParseFunc =
    ServerBlock::_initDirectivesMap();

directivesMap ServerBlock::_initLocationDirectivesMap()
{
    directivesMap tmpMap;

    tmpMap["root"] = &ServerBlock::addRoot;
    tmpMap["autoindex"] = &ServerBlock::addAutoIndex;
    tmpMap["allow_methods"] = &ServerBlock::addAllowedMethods;
    tmpMap["error_page"] = &ServerBlock::addErrorPage;
    tmpMap["client_body_buffer_size"] = &ServerBlock::addClientBodyBufferSize;
    tmpMap["cgi_param"] = &ServerBlock::addCgiParam;
    tmpMap["cgi_pass"] = &ServerBlock::addCgiPass;
    tmpMap["index"] = &ServerBlock::addIndex;
    tmpMap["alias"] = &ServerBlock::addAlias;

    return (tmpMap);
}

directivesMap ServerBlock::locationDirectivesParseFunc =
    ServerBlock::_initLocationDirectivesMap();

// default conf 읽고 파싱해서 ServerBlock클래스 공통 멤버변수 _defaultConf에
// 넣어줌
void ServerBlock::_initDefaultServer(const char *filename)
{
    ServerBlock server;
    fileVector file;

    // file 읽어줌
    file = readFile(filename);
    if (file.empty())
    {
        std::cerr << "Could not open default file at location [" << filename
                  << "]" << std::endl;
        throw FileNotFoundException();
    }
    fileVector begin;
    unsigned int index = 2;
    // 파일 내용 파싱하고 잘못되면 에러처리
    if (!server.parseServerBlock(index, file))
    {
        std::cerr << "Invalid default config file." << std::endl;
        throw ServerBlock::ExceptionInvalidArguments();
    }
    // ServerBlock class공통(static) _defaultConf에 server(원본 파일 내용이
    // 인자값으로 있는 것) 넣어줌
    ServerBlock::_default_conf = server;
}

// add 함수들

void ServerBlock::addServerName(std::vector<std::string> args)
{
    // 에러처리
    if (args.size() == 0)
        throw ExceptionInvalidArguments();
    // _server_name 값 넣어줌
    for (unsigned int i = 0; i < args.size(); i++)
        _server_name.push_back(args[i]);
}

void ServerBlock::addListen(std::vector<std::string> args)
{
    t_listen listen;
    size_t separator;

    // are는 무조건 1개 아니면 에러
    if (args.size() != 1)
        throw ExceptionInvalidArguments();
    // ':'가 없을 경우, (+ separator에 ":"대입)
    if ((separator = args[0].find(":")) == std::string::npos)
    {
        // 첫번째가 숫자면
        if (isDigits(args[0]))
        {
            // 호스트에 0넣고, port에 args[0] 내용9(8000) 복붙
            listen.host = 0;
            listen.port = atoi(args[0].c_str());
            // 자기 자신 _listen과 파싱한 listen의 port를 비교함
            for (std::vector<t_listen>::const_iterator it = _listen.begin();
                 it != _listen.end(); it++)
            {
                // _listen(이미 파싱한 값)과 listen(지금 들어온 값)이 같으면
                // 에러(중복 값)
                if (it->port == listen.port)
                    throw ExceptionInvalidArguments();
            }
            // _listen에 listen 넣음.
            _listen.push_back(listen);
            return;
        }
        throw ExceptionInvalidArguments();
    }
    // ':'가 있을 경우 (ip주소 있을 경우)
    else
    {
        // separator까지 주소 파싱해서 listen.host에 uint 포인터로 넣기,
        // localhost도 127.0.0.1로 바꿔줌
        listen.host = strToIp(args[0].substr(0, separator));
        separator++;
        // portStr에 그 이후 값의 주소 값 넣기
        std::string portStr = args[0].substr(separator);
        // portStr이 숫자이면
        if (isDigits(portStr))
        {
            // portStr을 char포인터로 바꿔서 atoi해서 port에 넣기
            listen.port = atoi(portStr.c_str());
            // listen을 값 넣어주기
            _listen.push_back(listen);
            return;
        }
        throw ExceptionInvalidArguments();
    }
}

void ServerBlock::addRoot(std::vector<std::string> args)
{
    // _root가 비어있거나 size값이 1개가 아니면 에러
    if (args.size() != 1 || _root != "")
        throw ExceptionInvalidArguments();
    // 값 넣어줌
    _root = args[0];
}

void ServerBlock::addAllowedMethods(std::vector<std::string> args)
{
    if (args.empty())
        throw ExceptionInvalidArguments();
    _allowed_methods.clear();
    for (std::vector<std::string>::iterator i = args.begin(); i != args.end();
         i++)
    {
        // 중복 값 들어올 시 에러 -> 필요 없다고 판단되면 지워도 됨
        if (_allowed_methods.count(*i))
            throw ExceptionInvalidArguments();
        _allowed_methods.insert(*i);
    }
}

void ServerBlock::addErrorPage(std::vector<std::string> args)
{
    std::vector<int> codes;
    std::string uri = "";
    size_t len = args.size();

    for (size_t i = 0; i < len; i++)
    {
        if (isDigits(args[i]))
            codes.push_back(atoi(args[i].c_str()));
        else if (codes.empty())
            throw ExceptionInvalidArguments();
        else if (i == len - 1)
            uri = args[i];
        else
            throw ExceptionInvalidArguments();
    }
    if (uri == "")
        throw ExceptionInvalidArguments();
    for (std::vector<int>::iterator i = codes.begin(); i != codes.end(); i++)
        this->_error_page[*i] = uri;
}

void ServerBlock::addClientBodyBufferSize(std::vector<std::string> args)
{
    if (args.size() != 1 || !isDigits(args[0]))
        throw ExceptionInvalidArguments();
    this->_client_body_buffer_size = atoi(args[0].c_str());
}

void ServerBlock::addAutoIndex(std::vector<std::string> args)
{
    if (args.size() != 1)
        throw ExceptionInvalidArguments();
    if (args[0] == "on")
        this->_auto_index = true;
    else if (args[0] == "off")
        this->_auto_index = false;
    else
        throw ExceptionInvalidArguments();
}

void ServerBlock::addCgiParam(std::vector<std::string> args)
{
    if (args.size() != 2)
        throw ExceptionInvalidArguments();

    this->_cgi_param[args[0]] = args[1];
}

void ServerBlock::addCgiPass(std::vector<std::string> args)
{
    if (args.size() != 1)
        throw ExceptionInvalidArguments();
    this->_cgi_pass = args[0];
}

void ServerBlock::addIndex(std::vector<std::string> args)
{
    if (args.empty())
        throw ExceptionInvalidArguments();
    this->_index.insert(this->_index.end(), args.begin(), args.end());
}

void ServerBlock::addAlias(std::vector<std::string> args)
{
    if (args.size() > 1)
        throw ExceptionInvalidArguments();
    if (args.size())
        this->_alias = args[0];
    this->_aliasSet = true;
}

// get 함수들
std::vector<std::string> ServerBlock::getServerName() const
{
    return this->_server_name;
}

std::vector<t_listen> ServerBlock::getListen() const { return this->_listen; }

std::string ServerBlock::getRoot() const { return this->_root; }

std::set<std::string> ServerBlock::getAllowedMethods() const
{
    return this->_allowed_methods;
}

std::map<int, std::string> ServerBlock::getErrorPage() const
{
    return this->_error_page;
}

int ServerBlock::getClientBodyBufferSize() const
{
    return this->_client_body_buffer_size;
}

bool ServerBlock::getAutoIndex() const { return this->_auto_index; }

std::map<std::string, std::string> ServerBlock::getCgiParam() const
{
    return this->_cgi_param;
}

std::string ServerBlock::getCgiPass() const { return this->_cgi_pass; }

std::map<std::string, ServerBlock> ServerBlock::getLocation() const
{
    return this->_location;
}

std::vector<std::string> ServerBlock::getIndex() const { return this->_index; }

std::string ServerBlock::getAlias() const { return this->_alias; }

bool ServerBlock::getAliasSet() const { return this->_aliasSet; }

// serverBlock 파싱
int ServerBlock::parseServerBlock(unsigned int &index, fileVector &file)
{
    fileVector args;
    directivesMap::iterator iter;
    std::string directive;

    // arg를 매개 변수로 사용하여 지시어에 해당하는 함수를 호출합니다.
    // directivesParseFunc을 돌면서 관련 명령어가 있는 지 파싱
    for (; index < file.size() && file[index] != "}"; index++)
    {
        // directivesParseFunc에 file[index]에 해당하는 것이 없을 경우
        if ((iter = directivesParseFunc.find(file[index])) ==
            directivesParseFunc.end())
        {
            // 1. 만약 location이라면?
            if (file[index] == "location")
            {
                ServerBlock location;
                std::string locationName;
                // directive가 빈 문자열이 아니라면?
                if (directive != "")
                {
                    // 해당 명령어 함수 동작시키고 arg, directive 초기화
                    (this->*ServerBlock::directivesParseFunc[directive])(args);
                    args.clear();
                    directive = "";
                }
                // 다음 단위가 '{' 또는 '}' 라면 리턴 0 (처음 반복문을 돌 때는
                // locationName자리임.)
                index++;
                if (file[index] == "{" || file[index] == "}")
                    return 0;
                // 아니라면 다음 인덱스는 locationName임.
                locationName = file[index];
                // locationName 다음에 있는 '{' 인덱스부터 parseLocationBlock
                // 보내 파싱해줌 -> 잘못되면 리턴 0
                index++;

                if (!location.parseLocationBlock(index, file))
                    return 0;
                // 파싱한 값 -> _location에 넣어줌
                _location[locationName] = location;
                // 다음 index값이 "}"라면 조건문 처음으로
                if (file[index] == "}")
                    continue;
            }
            else if (!directive.compare(""))
                return file[index] == "}" ? 1 : 0;
            else
            {
                args.push_back(file[index]);
            }
        }
        else
        {
            // directive에 값이 있을 때 실행함
            if (directive != "")
            {
                (this->*ServerBlock::directivesParseFunc[directive])(args);
                args.clear();
            }
            // directive에 해당 명령어를 넣음.
            directive = iter->first;
        }
    }
    // directive가 있으면 해당 명령어 실행
    if (directive != "")
        (this->*ServerBlock::directivesParseFunc[directive])(args);
    //  해당 서버 디렉토리스가 파싱이 끝났다면('}'을 만났다면)
    if (!file[index].compare("}"))
    {

        std::cout << "_root: " << _root << std::endl;
        std::cout << "cgi: " << _cgi_pass << std::endl;
        // 혹시나 빈 값 defaultServer conf값으로 채워줌
        ServerBlock::_default_conf.passMembers(*this);
        // location도 돌면서 값 채워줌
        for (std::map<std::string, ServerBlock>::iterator i = this->_location.begin(); i != this->_location.end(); i++)
            this->passMembers(i->second);
        

        std::cout << "_root2: " << _root << std::endl;
        std::cout << "cgi2: " << _cgi_pass << std::endl;
        return 1;
    }
    return 0;
};

// Location 파싱
int ServerBlock::parseLocationBlock(unsigned int &index, fileVector &file)
{
    fileVector args;
    directivesMap::iterator iter;
    std::string directive = "";

    if (file[index++] != "{")
        return 0;
    //	calling the function that corresponds to a directive with its args as
    // parameters
    for (; index < file.size() && file[index] != "}"; index++)
    {
        std::cout << index << ": " << file[index] << std::endl;

        // locationParsingMap에 해당 명령어 없을 경우
        if ((iter =
                 ServerBlock::locationDirectivesParseFunc.find(file[index])) ==
            ServerBlock::locationDirectivesParseFunc.end())
        {
            // 해당 명령어가 location일 경우
            if (file[index] == "location")
            {
                ServerBlock location;
                std::string locationName;

                if (directive != "")
                {
                    (this->*ServerBlock::locationDirectivesParseFunc
                                [directive])(args);
                    args.clear();
                    directive = "";
                }
                index++;

                if (file[index] == "{" || file[index] == "}")
                    return 0;
                locationName = file[index];
                index++;

                if (!location.parseLocationBlock(index, file))
                    return 0;
                _location[locationName] = location;
                if (file[index] == "}")
                    continue;
            }
            // location 아니고 directive 빈칸일 경우 -> 끝났거나 에러 1 또는 0
            // 리턴
            else if (directive == "")
                return file[index] == "}" ? 1 : 0;
            // location 아니고 directive 있을 경우 -> arg에 넣어주기
            else
                args.push_back(file[index]);
        }
        // locationParsingMap애 해당 명령어 있을 경우
        else
        {
            // directive이 있으면 실행
            if (directive != "")
            {
                (this->*ServerBlock::locationDirectivesParseFunc[directive])(
                    args);
                args.clear();
            }
            // directive없으면 해당 명령어 넣기
            directive = iter->first;
        }
    }
    if (directive != "")
        (this->*ServerBlock::locationDirectivesParseFunc[directive])(args);
    //  set up default values if they were not set by the config file
    if (!file[index].compare("}"))
        return 1;
    return 0;
}

// 서버의 빈 값 default서버로 채워줌 -> 여기서 this가 defaultserver conf값임
void ServerBlock::passMembers(ServerBlock &server) const
{
    // 서버랑 같으면 안됑
    if (this != &server)
    {
        // 서버 _listen이 비었니
        if (server._listen.empty())
            //this 클래스의 _listen값으로 채워줌
            server._listen.insert(server._listen.begin(), this->_listen.begin(), this->_listen.end());
        // 서버 _root 비었니
        if (server._root == "")
            // this 루트 넣어줌
            server._root = this->_root;
        // _server_name에 this값 넣어줌
        server._server_name.insert(server._server_name.end(), this->_server_name.begin(), this->_server_name.end());
        // _error_page 넣어줌
        for (std::map<int, std::string>::const_iterator i = this->_error_page.begin(); i != this->_error_page.end(); i++)
        {
            if (server._error_page.find(i->first) == server._error_page.end())
                server._error_page[i->first] = i->second;
        }
        // _client_body_buffer_size 넣어줌
        if (server._client_body_buffer_size == 0)
            server._client_body_buffer_size = this->_client_body_buffer_size;
        // _cgi_param 넗어줌
        for (std::map<std::string, std::string>::const_iterator i = this->_cgi_param.begin(); i != this->_cgi_param.end(); i++)
        {
            if (server._cgi_param.find(i->first) == server._cgi_param.end())
                server._cgi_param[i->first] = i->second;
        }
        // _cgi_pass 넣어줌
        if (server._cgi_pass == "")
            server._cgi_pass = this->_cgi_pass;
        // _allowed_methods 넣어줌
        if (server._allowed_methods.empty())
            server._allowed_methods = this->_allowed_methods;
        // _index 넣어줌
        server._index.insert(server._index.begin(), this->_index.begin(), this->_index.end());
    }
    // _location 넣어줌
    for (std::map<std::string, ServerBlock>::iterator i = server._location.begin(); i != server._location.end(); i++)
        server.passMembers(i->second);
}

const char *ServerBlock::ExceptionInvalidArguments::what()
    const throw()
{
    return "Exception: invalid arguments in configuration file";
}

// GET CONFIG FOR HTTP REQUEST
ServerBlock ServerBlock::getLocationForRequest(std::string const path, std::string &retLocationPath)
{
    std::string::size_type tryLen = path.length();
    std::map<std::string, ServerBlock>::iterator iter;
    std::string tryLocation;

    if (!tryLen)
        return *this;

    //location이 존재하면
    if (!this->_location.empty())
    {
        do
        {
            tryLocation = path.substr(0, tryLen);
            iter = this->_location.find(tryLocation);
            if (iter != this->_location.end() && iter->first[0] != '*')
            {
                retLocationPath = tryLocation;
                return iter->second.getLocationForRequest(path, retLocationPath); //ServerBlock(serverblock)
            }
            tryLen--;
        } while (tryLen);
        for (std::map<std::string, ServerBlock>::iterator i = this->_location.begin(); i != this->_location.end(); i++)
        {
            if (i->first[0] == '*')
            {
                std::string suffix = i->first.substr(1);
                if (path.length() > suffix.length() && !path.compare(path.length() - suffix.length(), suffix.length(), suffix))
                {
                    return i->second.getLocationForRequest(path, retLocationPath);
                }
            }
        }
    }
    return (*this);
}
