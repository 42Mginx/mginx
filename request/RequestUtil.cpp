#include "RequestUtil.hpp"

// readline 예시
// 1 GET /cgi-bin/http_trace.pl HTTP/1.1\r\n
// 2 ACCEPT_ENCODING: gzip,deflate,sdch\r\n
// 3 CONNECTION: keep-alive\r\n
// 4 ACCEPT: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n
// 5 ACCEPT_CHARSET: windows-949,utf-8;q=0.7,*;q=0.3\r\n
// 6 USER_AGENT: Mozilla/5.0 (X11; Linux i686) AppleWebKit/535.1 (KHTML, like
// Gecko) Chrome/13.0.782.24\r\n 7 ACCEPT_LANGUAGE:
// ko-KR,ko;q=0.8,en-US;q=0.6,en;q=0.4\rn 8 HOST: www.joinc.co.kr\r\n 9 \r\n

// 한 줄 파싱해주는 역할
std::string readLine(const std::string &str, size_t &i) {
    std::string ret;
    size_t j;

    // size가 npos이면 빈 문자열
    if (i == std::string::npos)
        return "";
    // i 이후의(i부터 시작) 처음 나온 '\n'인덱스 j에 저장,
    j = str.find_first_of('\n', i);
    // 그 한 줄 ret에 저장 (\r까지 저장)
    // 만약 j가 npos(= -1)이라면, substr은 총 길이에서 (j - i)만큼을 자르기
    // 때문에 ret은 결국에 빈 문자열
    ret = str.substr(i, j - i);
    // 마지막 문자열이 '\r'이면 제거 -> 이 과정을 거치고 나면 \r\n이 없는 한
    // 줄이 나옴.
    if (ret[ret.size() - 1] == '\r')
        pop(ret);
    // j가 std::string::npos이면(str에 \n이 없었을 경우) j(='npos'), 있으면 j +
    // 1(='\n'의 다음 인덱스), i에 대입
    i = (j == std::string::npos ? j : j + 1);
    // ret 리턴
    return ret;
}

// 스트링 맨 뒤 한 글자 제거해주는 함수.
std::string &pop(std::string &str) {
    if (str.size())
        str.resize(str.size() - 1);
    return str;
}

// key 파싱
std::string readKey(const std::string &line) {
    std::string ret;

    // ':' 전까지 붙여넣기
    size_t i = line.find_first_of(':');
    ret.append(line, 0, i);
    // ret 첫글자 대문자화
    capitalize(ret);
    // 띄어쓰기 앞뒤로 잘라서 리턴
    return (strip(ret, ' '));
}

std::string &capitalize(std::string &str) {
    size_t i = 0;

    // str 전부 소문자로 바꿈
    to_lower(str);
    // str[0] 대문자로 변경
    str[i] = std::toupper(str[i]);
    // '-' 위치 찾아서 그 다음문자 대문자로 변경해줌 (글자 범위 안에서)
    while ((i = str.find_first_of('-', i + 1)) != std::string::npos) {
        if (i + 1 < str.size())
            str[i + 1] = std::toupper(str[i + 1]);
    }
    // 리턴
    return str;
}

// Value 파싱
std::string readValue(const std::string &line) {
    size_t i;
    std::string ret;

    // ':' 위치 찾기
    i = line.find_first_of(':');
    // i+1부터 ' '가 아닌 게 나올때 위치(':' 이후 첫 문자열 위치 찾기)
    i = line.find_first_not_of(' ', i + 1);
    // 문자열이 있으면 끝까지 전부 ret에 넣어줌
    if (i != std::string::npos)
        ret.append(line, i, std::string::npos);
    // 앞뒤로 빈칸 잘라줌
    return (strip(ret, ' '));
}

std::string &strip(std::string &str, char c) {
    size_t i;

    if (!str.size())
        return str;
    i = str.size();
    while (i && str[i - 1] == c)
        i--;
    str.resize(i);
    for (i = 0; str[i] == c; i++)
        ;
    str = str.substr(i, std::string::npos);
    return str;
}

std::string &to_upper(std::string &str) {
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

std::string &to_lower(std::string &str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}