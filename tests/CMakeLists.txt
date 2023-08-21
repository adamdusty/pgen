cmake_minimum_required(VERSION 3.25)

project(
    pgenTests
    LANGUAGES CXX
)

if(PROJECT_IS_TOP_LEVEL)
    find_package(pgen REQUIRED)
    enable_testing()
endif()

find_package(Catch2 3 REQUIRED)

add_executable(pgen_tests
    src/pgen_test.cpp
)

target_link_libraries(pgen_tests PRIVATE pgen_lib Catch2::Catch2WithMain)

add_test(NAME pgen_tests COMMAND pgen_tests)