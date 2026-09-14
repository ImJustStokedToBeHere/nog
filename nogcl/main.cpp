#include "nog/nog.h"
#include <iostream>
#include <ostream>

int main(int argc, char** argv) {
    auto tester_result = nog::tester();

    std::cout << "tester_result: " << tester_result << std::endl;

    return 1;
}
