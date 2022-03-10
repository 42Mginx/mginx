#include "./ServerBlock.hpp"

int	ServerBlock::parseServerBlock(std::string config_path)
{
	fileVector                  args;
	parseMap::iterator          iter;
	std::string                 directive;

	// arg를 매개 변수로 사용하여 지시어에 해당하는 함수를 호출합니다.
	//serverParsingMap을 돌면서 관련 명령어가 있는 지 파싱
	for ( ; index < file.size() && file[index] != "}" ; index++) {
		// serverParsingMap에 file[index]에 해당하는 것이 없을 경우
		if ((iter = ConfigServer::serverParsingMap.find(file[index])) == ConfigServer::serverParsingMap.end()) {
			
			// 1. 만약 location이라면?
			if (file[index] == "location") {
				ConfigServer	location;
				std::string	locationName;

				// directive가 빈 문자열이 아니라면?
				if (directive != "") {
					// 해당 명령어 함수 동작시키고 arg, directive 초기화
					(this->*ConfigServer::serverParsingMap[directive])(args);
					args.clear();
					directive = "";
				}
				// 다음 단위가 '{' 또는 '}' 라면 리턴 0 (처음 반복문을 돌 때는 locationName자리임.)
				index++;
				if (file[index] == "{" || file[index] == "}")
					return 0;
				// 아니라면 다음 인덱스는 locationName임.
				locationName = file[index];
				// locationName 다음에 있는 '{' 인덱스부터 parseLocation으로 보내 파싱해줌 -> 잘못되면 리턴 0
				index++;
				if (!location.parseLocation(index, file))
					return 0;
				// 파싱한 값 -> _location에 넣어줌
				this->_location[locationName] = location;
				// 다음 index값이 "}"라면 조건문 처음으로
				if (file[index] == "}")
					continue ;
			}

			// location이 아니고 directive가 빈 문자열이라면(없다면) 해당 값이 "}"인지 확인 후에 1 또는 0 리턴
			else if (!directive.compare(""))
				return file[index] == "}" ? 1 : 0;
			
			// 그 외 -> 해당 file[index]를 args에 push_back해줌.
			else
				args.push_back(file[index]);
		}
		// 해당하는 명령어가 serverParsingMap에 있을 경우
		else
		{
			// directive에 값이 있을 때 실행함
			if (directive != "") {
				(this->*ConfigServer::serverParsingMap[directive])(args);
				args.clear();
			}
			// directive에 해당 명령어를 넣음.
			directive = iter->first;
		}
	}
	// directive가 있으면 해당 명령어 실행
	if (directive != "")
		(this->*ConfigServer::serverParsingMap[directive])(args);
	//  해당 서버 디렉토리스가 파싱이 끝났다면('}'을 만났다면)
	if (!file[index].compare("}")) {
		// 혹시나 빈 값 defaultServer conf값으로 채워줌
		ConfigServer::_defaultServer.passMembers(*this);
		// location도 돌면서 값 채워줌
		for (std::map<std::string, ConfigServer>::iterator i = this->_location.begin() ; i != this->_location.end(); i++)
			this->passMembers(i->second);
		return 1;
	}
	return 0;
}