#include <iostream>

int		main(int argc, char **argv)
{
	if (argc == 2)
	{
		try {
            // parsing 추가
			if (setup() == -1) // listen까지 진행
				return (1);
			run();
			// _mginx.clean();
		}
		catch (...) {
			// 대충 에러문
            std::cerr << "[error!!]" << std::endl;
		}
	
	}
	else
	{
		std::cerr  << "Invalid number of arguments." << std::endl;
		return (1);	
	}

	return (0);
}