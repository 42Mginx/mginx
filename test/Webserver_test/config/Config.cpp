#include "Config.hpp"

Config::Config() {
    _initDefaultServer(DEFAULT_CONFIG_PATH);
}

Config::Config(std::string config_path) {
    _initDefaultServer(DEFAULT_CONFIG_PATH);
    parseProcess(config_path);
    parseAllListens();
}

Config::~Config() {}

int Config::parseProcess(std::string config_path) {
    fileVector file;
    unsigned int fileSize;

    std::cout << "testing// config parse process" << std::endl;
    file = readFile(config_path);
    fileSize = file.size();
    for (unsigned int i = 0; i < fileSize; i++) {
        if (file[i] == "server") {
            ServerBlock tmpServerBlock;
            ++i;
            if (file[i] != "{") {
                std::cerr << "Error: expected '{' after server directive." << std::endl;
                return 1;
            }
            ++i;
            if (!tmpServerBlock.parseServerBlock(i, file)) {
                std::cerr << "Error: error in config file [" << config_path << "]" << std::endl;
                return 1;
            }
            this->_serverBlocks.push_back(tmpServerBlock);
        } else {
            std::cerr << "Error: unknown directive [" << file[i] << "]" << std::endl;
            return 1;
        }
    }
    return 0;
}

// default conf 읽고 파싱해서 ServerBlock클래스 공통 멤버변수 _defaultConf에 넣어줌
ServerBlock Config::_initDefaultServer(const char *filename) {
    ServerBlock server;
    fileVector file;

    // file 읽어줌
    file = readFile(filename);
    if (file.empty()) {
        std::cerr << "Could not open default file at location [" << filename << "]" << std::endl;
        // throw FileNotFoundException();
    }
    fileVector begin;
    unsigned int index = 0;
    // 파일 내용 파싱하고 잘못되면 에러처리
    if (!server.parseServerBlock(index, file)) {
        std::cerr << "Invalid default config file." << std::endl;
        // throw ServerBlock::ExceptionInvalidArguments();
    }
    // ServerBlock class공통(static) _defaultConf에 server(원본 파일 내용이 인자값으로 있는 것) 넣어줌
    _defaultConf = server;
    return server;
}

// file 이름 받아서 읽은 값을 '띄어쓰기 엔터랑 탭' 단위로 쪼개서 vector에 넣어준 후 리턴해줌
fileVector Config::readFile(std::string config_path_str) {
    int ret = READER_BUFFER_SIZE;
    char buffer[READER_BUFFER_SIZE + 1];
    std::string line = "";
    int fd;
    fileVector file;
    const char *config_path;

    // 버퍼 초기화
    for (int i = 0; i < READER_BUFFER_SIZE + 1; i++)
        buffer[i] = '\0';

    // str -> char*로 변환
    config_path = config_path_str.c_str();
    std::cout << config_path << std::endl;
    // 파일 오픈 -> 실패시 에러 throw
    if ((fd = open(config_path, O_RDONLY)) <= 0) {
        // TODO: 인자로 잘못된 path가 들어올 경우 terminating으로 끝남
        std::cout << "testing// config => file open error" << std::endl;
        throw;
    }

    // line에 read해서 계속해서 값 넣어주기
    for (ret = READER_BUFFER_SIZE; ret > 0; ret = read(fd, buffer, READER_BUFFER_SIZE)) {
        buffer[ret] = '\0';
        line += buffer;
    }

    //	read 에러 발생시 => 에러 출력 후 빈 file 리턴 출력
    if (ret == -1) {
        std::cerr << "Error while reading config file." << std::endl;
        return file;
    }

    // line에서 띄어쓰기(' ') 엔터('\n')랑 탭('\t') 단위로 쪼개서 vector에 넣어준 후 리턴해줌
    file = split(line, std::string(" \n\t"));
    return file;
}

// charset안의 char(한 단어단어)와 일치하는 부분 단위로 string으로 쪼개서 fileVector에 넣어 리턴해줌

fileVector Config::split(std::string str, std::string charset) {
    fileVector tokens;

    // str에 charset[0] 추가
    str += charset[0];

    // charset과 str 틀린 부분 찾기
    std::string::size_type start = str.find_first_not_of(charset, 0);
    std::string::size_type end = 0;

    while (true) {
        // charset과 str 틀린 부분 부터 str과 charset 같은 부분 인덱스 찾기
        end = str.find_first_of(charset, start);

        // end == npos 라는 뜻은 같은 부분이 없다는 뜻이고 그 말은 end가 곧 str끝이라는 뜻이니 while문 탈출
        if (end == std::string::npos) {
            break;
        }

        // s에 str과 charset 다른 부분 넣기
        std::string s = str.substr(start, end - start);

        // tonks에 s 밀어 넣기
        tokens.push_back(s);

        // charset과 str 틀린 부분을 다시 찾음 혹시 없으면 break;
        if ((start = str.find_first_not_of(charset, end)) == std::string::npos)
            break;
    }
    // 토큰 리턴
    return tokens;
}

std::vector<t_listen> Config::parseAllListens() const {
    std::vector<t_listen> allListens;

    // 서버블록을 순환
    for (std::vector<ServerBlock>::const_iterator serverBlock = _serverBlocks.begin(); serverBlock != _serverBlocks.end(); serverBlock++) {
        // listen 백터를 순환
        std::vector<t_listen> listenVec = serverBlock->getListen();
        for (std::vector<t_listen>::iterator listen = listenVec.begin(); listen != listenVec.end(); listen++) {
            std::vector<t_listen>::iterator i = allListens.begin();
            for (; i != allListens.end(); i++)
                if (listen->host == i->host && listen->port == i->port)
                    break;
            if (i == allListens.end())
                allListens.push_back(*listen);
        }
    }
    return allListens;
}

ServerBlock Config::getDefaultConf() {
    return this->_defaultConf;
}

std::vector<ServerBlock> Config::getServerBlock() {
    return this->_serverBlocks;
}

std::vector<t_listen> Config::getAllListens() {
    return this->_allListens;
}