#include "lib.hpp"
#include <iostream>

auto main(int argc, char** argv) -> int {
    std::cout << pgen::greet() << '\n';
    return 0;
}