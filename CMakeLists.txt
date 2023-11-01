cmake_minimum_required(VERSION 3.27)

project(
    pgen
    VERSION 0.2.1
    DESCRIPTION "A short description"
    LANGUAGES CXX
)

include(cmake/build-options.cmake)
include(cmake/variables.cmake)
include(cmake/dependencies.cmake)

add_library(
    pgen_lib OBJECT
    src/serial.cpp
    src/write.cpp
    src/render.cpp
    src/cmd.cpp
)

target_include_directories(
    pgen_lib ${warning_guard}
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)

target_link_libraries(pgen_lib PUBLIC nlohmann_json::nlohmann_json pantor::inja argparse::argparse)

add_executable(
    pgen_exe
    src/main.cpp
)

target_link_libraries(pgen_exe PRIVATE pgen_lib)

set_target_properties(
    pgen_exe PROPERTIES
    OUTPUT_NAME pgen
)

include(cmake/compile-options.cmake)

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

if(BUILD_PGEN_TESTS)
    include(CTest)
    add_subdirectory(tests)
endif()