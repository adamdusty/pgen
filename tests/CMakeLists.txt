cmake_minimum_required(VERSION 3.25)

project(
    pgenTests
    LANGUAGES CXX
)

if(PROJECT_IS_TOP_LEVEL)
    find_package(pgen REQUIRED)
    enable_testing()
endif()

include(FetchContent)
FetchContent_Declare(
    snitch
    GIT_REPOSITORY https://github.com/cschreib/snitch.git
    GIT_TAG        v1.2.2
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(snitch)

add_executable(pgen_tests
    src/pgen_test.cpp
)

target_link_libraries(pgen_tests PRIVATE pgen_lib snitch::snitch)

add_test(NAME pgen_tests COMMAND pgen_tests)