#include "./ConfigUtil.hpp"

bool isDigits(const std::string &str) {
	return str.find_first_not_of("0123456789") == std::string::npos;
}

// IP주소값을 str으로 받아서 uint포인터로 리턴을 해줌('.'은 뺴고)
unsigned int	strToIp(std::string strIp) {
	size_t  sep = 0;
	unsigned int   n;
	unsigned char  m[4];
	size_t  start = 0;
	if (strIp == "localhost")
		strIp = "127.0.0.1";
	// 아이피 주소를 .을 기준으로 쪼개서 m에 넣음(char형 크기가 255라서 char로 받는 듯)
	// m = split('.')
	for (unsigned int i = 0 ; i < 4; i++) {
		// '.'을 찾음
		sep = strIp.find_first_of('.', start);
		// 시작부터 '.'까지 문자들 파싱
		std::string str = strIp.substr(start, sep - start);
		// str을 const char*로 바꾼 뒤 int형으로 바꿈
		n = atoi(str.c_str());
		// n을 다시 unsigned_char로 캐스팅해서 m[i]에 넣음
		m[i] = static_cast<unsigned char>(n);
		std::cout << m[i] << std::endl;
		// '.'다음으로 넘김
		sep++;
		// start 다시 지정해줌
		start = sep;
	}
	// m을 int*로 변환해서 리턴해줌
	unsigned final = *(reinterpret_cast<unsigned int *>(m));
	return final;
}