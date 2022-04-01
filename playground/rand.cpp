#include <cstdlib>
#include <ctime>
#include <iostream>

class Test {
   public:
    Test() {
        std::cout << "=> 생성" << std::endl;
    };
    ~Test() {
        std::cout << "=> 파괴" << std::endl;
    }
    int getRand() {
        srand((unsigned int)time(NULL));
        std::cout << "=> 랜덤" << std::endl;
        return rand();
    }
};

int main() {
    Test test;
    int num = test.getRand();
    int num2 = test.getRand();
    int num3 = test.getRand();
    std::cout << num << std::endl;
    std::cout << num2 << std::endl;
    std::cout << num3 << std::endl;
}