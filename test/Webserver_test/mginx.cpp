#include "Webserver.hpp"

int main(int argc, char **argv) {
    Webserver webserver;
    int ret;

    if (argc == 2) {
        try {
            webserver.parseConfig(argv[1]);
            ret = webserver.setup();
            if (ret == -1) {
                std::cout << "webserver setup error" << std::endl;
            }
            if (webserver.run() == -1) {
                std::cout << "webserver setup error" << std::endl;
            }
        } catch (...) {
            std::cerr << "error occured in process" << std::endl;
        }
    } else {
        std::cerr << "Please enter the config file path" << std::endl;
    }
}