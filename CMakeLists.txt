cmake_minimum_required(VERSION 3.25)

project(
    pgen
    VERSION 0.1.0
    DESCRIPTION "A short description"
    LANGUAGES CXX
)

option(PGEN_TEST "Build tests" OFF)

include(cmake/variables.cmake)
include(cmake/dependencies.cmake)

add_library(
    pgen_lib OBJECT
    src/lib.cpp
)

target_include_directories(
    pgen_lib ${warning_guard}
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)

target_include_directories(
    pgen_lib SYSTEM
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_link_libraries(pgen_lib PUBLIC fmt::fmt tomlplusplus::tomlplusplus argparse::argparse)

add_executable(
    pgen_exe
    src/main.cpp
)

target_compile_definitions(pgen_exe PRIVATE -DTOML_HEADER_ONLY=0)
target_link_libraries(pgen_exe PRIVATE pgen_lib)

set_target_properties(
    pgen_exe PROPERTIES
    OUTPUT_NAME pgen
)

include(cmake/compile-options.cmake)

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

if(PGEN_TEST)
    include(CTest)
    add_subdirectory(tests)
endif()