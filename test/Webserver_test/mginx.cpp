#include "Webserver.hpp"

int main() {
    Webserver webserver;
    int ret;

    ret = webserver.setup();
    if (ret == -1) {
        std::cout << "webserver setup error" << std::endl;
    }
    if (webserver.run() == -1) {
        std::cout << "webserver setup error" << std::endl;
    }
}