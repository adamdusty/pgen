cmake_minimum_required(VERSION 3.25)

project(
    pgen
    VERSION 0.1.0
    DESCRIPTION "A short description"
    LANGUAGES CXX
)

include(cmake/variables.cmake)

find_package(fmt REQUIRED)
find_package(tomlplusplus REQUIRED)
find_package(argparse REQUIRED)

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

target_precompile_headers(
    pgen_lib PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>/inja.hpp"
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>/json.hpp"
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



if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

target_compile_options(pgen_exe PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wcast-qual
        -Wundef
        # -Werror=float-equal
        -Wshadow
        -Wcast-align
        -Wunused
        -Wnull-dereference
        -Wdouble-promotion
        -Wimplicit-fallthrough
        -Woverloaded-virtual
        -Wnon-virtual-dtor
        -Wold-style-cast
    >

    $<$<CXX_COMPILER_ID:MSVC>:
        /utf-8
        /w14165
        /w44242
        /w44254
        /w44263
        /w34265
        /w34287
        /w44296
        /w44365
        /w44388
        /w44464
        /w14545
        /w14546
        /w14547
        /w14549
        /w14555
        /w34619
        /w34640
        /w24826
        /w14905
        /w14928
        /w45038
        /w44242
        /persmissive-
        /volatile:iso
        /Zc:preprocessor
        /Zc:__cplusplus
        /Zc:externConstexpr
        /Zc:throwingNew
        /EHsc
    >
)

if(PGEN_TEST)
    include(CTest)
    add_subdirectory(tests)
endif()