#include "Mginx.hpp"

int		main(int argc, char **argv)
{
	if (argc == 2)
	{
		Mginx	_mginx;

		try {
			_mginx.config(argv[1]);
			if (_mginx.setup() == -1)
				return (1);
			_mginx.run();
			_mginx.clean();
		}
		catch () {
			// 대충 에러문
		}
	
	}
	else
	{
		std::cerr << RED << "Invalid number of arguments." << RESET << std::endl;
		return (1);	
	}

	return (0);
}