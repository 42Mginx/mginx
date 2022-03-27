#include <stdlib.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

std::map<int, std::string> getMap() {
    std::map<int, std::string> a;
    a.insert(std::pair<int, std::string>(42, "forty two"));
    return a;
}

void increase(int &num) { num++; }

int main(int argc, char **argv) {
    if (argc == 2) {
        std::string string = argv[1];
        size_t a = strtol(string.c_str(), NULL, 16);
        size_t b = 13;

        std::cout << "a is " << a << std::endl;
        std::cout << "b is " << b << std::endl;
        std::cout << "a == b" << (a == b) << std::endl;
        std::cout << "a < b" << (a < b) << std::endl;
        std::cout << "a > b" << (a > b) << std::endl;
    }

    // std::map<int, std::string>::iterator it = getMap().begin(); // error
    // should work
    std::map<int, std::string> tmp = getMap();
    std::map<int, std::string>::iterator it = tmp.begin();

    std::vector<int> a;
    a.push_back(1);
    a.push_back(2);
    a.push_back(3);
    std::vector<int>::iterator it2 = a.begin();
    for (; it2 != a.end(); it2++) {
        increase(*it2);
    }
    it2 = a.begin();
    for (; it2 != a.end(); it2++) {
        std::cout << *it2 << std::endl;
    }
}