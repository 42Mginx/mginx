#include "Webserver.hpp"

int main(int argc, char **argv) {
    Webserver webserver;
    int ret;

    if (argc == 2) {
        try {
            webserver.parseConfig(argv[1]);
            webserver.init();
            ret = webserver.setup();
            if (ret == -1) {
                std::cout << "webserver setup error" << std::endl;
            }
            if (webserver.run() == -1) {
                std::cout << "webserver setup error" << std::endl;
            }
        } catch (std::exception &e) {
            std::cerr << "[Exception] error occured => " << e.what()
                      << std::endl;

        } catch (...) {
            std::cerr << "[Exception] error occured in process" << std::endl;
        }
    } else {
        std::cerr << "Please enter the config file path" << std::endl;
    }
}