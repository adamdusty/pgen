include(FetchContent)
FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG        master
    FIND_PACKAGE_ARGS
)


FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG        master
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(tomlplusplus argparse)

find_package(fmt REQUIRED)