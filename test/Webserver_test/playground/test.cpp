#include <stdlib.h>

#include <iostream>
#include <string>

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
}