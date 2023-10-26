cmake_minimum_required(VERSION 3.27)

project(
    benchmark
    LANGUAGES CXX
)

include(FetchContent)
FetchContent_Declare(
    nanobench
    GIT_REPOSITORY https://github.com/martinus/nanobench.git
    GIT_TAG        v4.3.11
    GIT_SHALLOW    TRUE
    SYSTEM
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(nanobench)

add_executable(bench
    src/bench.cpp
)

target_link_libraries(sort_bench PRIVATE pgen_lib nanobench::nanobench)