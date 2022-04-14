#include "./ConfigUtil.hpp"

bool isDigits(const std::string &str) {
    return str.find_first_not_of("0123456789") == std::string::npos;
}

// IP주소값을 str으로 받아서 uint포인터로 리턴을 해줌('.'은 뺴고)
unsigned int strToIp(std::string strIp) {
    size_t sep = 0;
    unsigned int n;
    unsigned char m[4];
    size_t start = 0;
    if (strIp == "localhost")
        strIp = "127.0.0.1";
    // 아이피 주소를 .을 기준으로 쪼개서 m에 넣음(char형 크기가 255라서 char로
    // 받는 듯) m = split('.')
    for (unsigned int i = 0; i < 4; i++) {
        // '.'을 찾음
        sep = strIp.find_first_of('.', start);
        // 시작부터 '.'까지 문자들 파싱
        std::string str = strIp.substr(start, sep - start);
        // str을 const char*로 바꾼 뒤 int형으로 바꿈
        n = atoi(str.c_str());
        // n을 다시 unsigned_char로 캐스팅해서 m[i]에 넣음
        m[i] = static_cast<unsigned char>(n);
        // '.'다음으로 넘김
        sep++;
        // start 다시 지정해줌
        start = sep;
    }
    // m을 int*로 변환해서 리턴해줌
    unsigned final = *(reinterpret_cast<unsigned int *>(m));
    return final;
}
// file 이름 받아서 읽은 값을 '띄어쓰기 엔터랑 탭' 단위로 쪼개서 vector에 넣어준
// 후 리턴해줌
fileVector readFile(std::string config_path_str) {
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
    // 파일 오픈 -> 실패시 에러 throw
    if ((fd = open(config_path, O_RDONLY)) <= 0)
        throw FileNotFoundException();

    // line에 read해서 계속해서 값 넣어주기
    for (ret = READER_BUFFER_SIZE; ret > 0;
         ret = read(fd, buffer, READER_BUFFER_SIZE)) {
        buffer[ret] = '\0';
        line += buffer;
    }

    //	read 에러 발생시 => 에러 출력 후 빈 file 리턴 출력
    if (ret == -1) {
        close(fd);
        std::cerr << "Error while reading config file." << std::endl;
        return file;
    }

    // line에서 띄어쓰기(' ') 엔터('\n')랑 탭('\t') 단위로 쪼개서 vector에
    // 넣어준 후 리턴해줌
    file = split(line, std::string(" \n\t"));
    close(fd);
    return file;
}

// charset안의 char(한 단어단어)와 일치하는 부분 단위로 string으로 쪼개서
// fileVector에 넣어 리턴해줌

fileVector split(std::string str, std::string charset) {
    fileVector tokens;

    // str에 charset[0] 추가
    str += charset[0];

    // charset과 str 틀린 부분 찾기
    std::string::size_type start = str.find_first_not_of(charset, 0);
    std::string::size_type end = 0;

    while (true) {
        // charset과 str 틀린 부분 부터 str과 charset 같은 부분 인덱스 찾기
        end = str.find_first_of(charset, start);

        // end == npos 라는 뜻은 같은 부분이 없다는 뜻이고 그 말은 end가 곧
        // str끝이라는 뜻이니 while문 탈출
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

const char *FileNotFoundException::what() const throw() {
    return "Exception thrown: could not open configuration file";
}