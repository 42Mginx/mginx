#include "Config.hpp"

Config::Config()
{
	// _defaultConf = parseProcess(DEFAULT_CONFIG_PATH);
}

Config::Config(std::string config_path)
{
	// _defaultConf = parseProcess(DEFAULT_CONFIG_PATH);
	parseProcess(config_path);
}

Config::~Config() {}

int			Config::parseProcess(std::string config_path)
{
	fileVector					file;
	unsigned int				fileSize;

	file = readFile(config_path);
	fileSize = file.size();
	for (unsigned int i = 0 ; i < fileSize; i++) {
		if (file[i] == "server") {
			ServerBlock					tmpServerBlock;
			++i;
			if (file[i] != "{") {
				std::cerr << "Error: expected '{' after server directive." << std::endl;
				return 1;
			}
			++i;
			if (!tmpServerBlock.parseServerBlock(i, file)) {
				std::cerr << "Error: error in config file [" << config_path << "]" <<  std::endl;
				return 1;
			}
			this->_serverBlock.push_back(tmpServerBlock);
		}
		else {
			std::cerr << "Error: unknown directive [" << file[i] << "]" << std::endl;
			return 1;
		}
	}
	return 0;
}

// file 이름 받아서 읽은 값을 '띄어쓰기 엔터랑 탭' 단위로 쪼개서 vector에 넣어준 후 리턴해줌
fileVector	Config::readFile(std::string config_path_str) {
	int							ret = READER_BUFFER_SIZE;
	char						buffer[READER_BUFFER_SIZE + 1];
	std::string					line = "";
	int							fd;
	fileVector					file;
	const char					*config_path;

	// 버퍼 초기화
	for (int i = 0; i < READER_BUFFER_SIZE + 1;  i++)
		buffer[i] = '\0';

	// str -> char*로 변환
	config_path = config_path_str.c_str();
	// 파일 오픈 -> 실패시 에러 throw
	if ((fd = open(config_path, O_RDONLY)) <= 0)
		throw ;

	// line에 read해서 계속해서 값 넣어주기
	for (ret = READER_BUFFER_SIZE; ret > 0; ret = read(fd, buffer,READER_BUFFER_SIZE )) {
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

//charset안의 char(한 단어단어)와 일치하는 부분 단위로 string으로 쪼개서 fileVector에 넣어 리턴해줌

fileVector				Config::split(std::string str, std::string charset) {
	fileVector	tokens;

	// str에 charset[0] 추가
	str += charset[0];
	
	// charset과 str 틀린 부분 찾기
	std::string::size_type	start = str.find_first_not_of(charset, 0);
	std::string::size_type	end = 0;

	while (true) {
		// charset과 str 틀린 부분 부터 str과 charset 같은 부분 인덱스 찾기
		end = str.find_first_of(charset, start);
		
		// end == npos 라는 뜻은 같은 부분이 없다는 뜻이고 그 말은 end가 곧 str끝이라는 뜻이니 while문 탈출
		if (end == std::string::npos) {
			break;
		}

		// s에 str과 charset 다른 부분 넣기
		std::string	s = str.substr(start, end - start);
		
		// tonks에 s 밀어 넣기 
		tokens.push_back(s);

		// charset과 str 틀린 부분을 다시 찾음 혹시 없으면 break;
		if ((start = str.find_first_not_of(charset, end)) == std::string::npos)
			break ;
	}
	// 토큰 리턴
	return tokens;
}