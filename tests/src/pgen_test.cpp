#include "lib.hpp"
#include <catch2/catch_test_macros.hpp>
#include <string>

TEST_CASE("Test case", "[greeting]") {
    auto expected = std::string{"Hello from lib"};
    REQUIRE(pgen::greet() == expected);
}