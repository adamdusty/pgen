cmake_minimum_required(VERSION 3.27)

project(
    pgenTests
    LANGUAGES CXX
)

# If test project is built independently, find the project package
if(PROJECT_IS_TOP_LEVEL)
    find_package(pgen_lib REQUIRED)
    enable_testing()
endif()

# Get the testing library
include(FetchContent)
FetchContent_Declare(
    snitch
    GIT_REPOSITORY https://github.com/snitch-org/snitch.git
    GIT_TAG        v1.2.4
    GIT_SHALLOW    TRUE
    SYSTEM
    FIND_PACKAGE_ARGS # Use system package if installed
)
FetchContent_MakeAvailable(snitch)

# Add test exe
add_executable(pgen_tests)

# Add test sources
target_sources(pgen_tests PRIVATE
    src/pgen_test.cpp
)

# Link libs
target_link_libraries(pgen_tests PRIVATE pgen_lib snitch::snitch)

# Add test
add_test(NAME pgen_tests COMMAND pgen_tests)

# Enable test binary to find the DLL if building a shared lib
if(BUILD_SHARED_LIBS)
    set_tests_properties(pgen_tests PROPERTIES
        ENVIRONMENT_MODIFICATION
            "PATH=path_list_append:$<TARGET_FILE_DIR:pgen_lib>;PATH=path_list_append:$<TARGET_FILE_DIR:snitch::snitch>"
    )
endif()
