#include "Webserver.hpp"

// 인자로 config 파일 path를 받음.

int main(int argc, char **argv) {
    // 웹서브 클래스 생성자 호출 (별 다른 호출 함수는 없음)
    Webserver webserver;
    int ret;

    // 인자가 path외에 다른 것이 들어온다면 에러
    if (argc == 2) {
        try {
            // path를 인자로 config 파싱(config 클래스의 parseProcess 함수 호출)
            webserver.parseConfig(argv[1]);
            // 인자 전부 0으로 초기화하고, 열어야하는 listen 포트를 config 클래스에서 getAllListens함수를 통해서 받음
            webserver.init();
            // 받아온 listen 백터를 돌면서 해당 listen에 대한 소켓 셋업 및 listen 포트 열기.
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
