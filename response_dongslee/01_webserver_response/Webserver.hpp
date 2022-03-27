#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP
// CPP Includes
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

// CPP Containers
#include <algorithm>
#include <iterator>
#include <list>
#include <map>
#include <set>
#include <utility>
#include <vector>

// C Includes
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// C System
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

// C Network
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>

// util
#define BUF_SIZE 4096

class Serverblock{
	private:
	std::string _test;

	public:
	Serverblock(){
		_test = "hello";

	};
	~Serverblock(){};

};


#endif
