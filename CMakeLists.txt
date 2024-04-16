cmake_minimum_required(VERSION 3.27)

project(
    pgen
    VERSION 0.1.0
    DESCRIPTION "A short description"
    LANGUAGES CXX
)

# Include utilities
include(cmake/options.cmake)
include(cmake/compile-options.cmake)
include(cmake/dependencies.cmake)

# Declare library
add_library(pgen_lib)

# Add include directory
target_include_directories(
    pgen_lib
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)

# Add sources
target_sources(pgen_lib PRIVATE
    src/lib.cpp
)

# Declare executable
add_executable(pgen)

# Add sources
target_sources(pgen PRIVATE
    src/main.cpp
)

# Include and link dependencies
target_link_libraries(pgen PRIVATE pgen_lib)

# Add compile options
add_dev_compile_opts(pgen_lib)
add_dev_compile_opts(pgen)


# Set properties
set_target_properties(
    pgen PROPERTIES
    OUTPUT_NAME pgen
)

# Install rules
if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# Enable testing
if(BUILD_TESTING)
    include(CTest)
    add_subdirectory(tests)
endif()