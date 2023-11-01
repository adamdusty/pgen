include(FetchContent)

set(JSON_BuildTests OFF CACHE INTERNAL "Don't build tests")
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
    SYSTEM
)

set(INJA_USE_EMBEDDED_JSON OFF CACHE INTERNAL "Use external json")
set(INJA_BUILD_TESTS OFF CACHE INTERNAL "Don't build tests")
set(BUILD_BENCHMARK OFF CACHE INTERNAL "Don't build benchmarks")
set(INJA_INSTALL OFF CACHE INTERNAL "Don't generate install targets")
FetchContent_Declare(
    inja
    GIT_REPOSITORY https://github.com/pantor/inja.git
    GIT_TAG        v3.4.0
    GIT_SHALLOW    TRUE
    SYSTEM
)

FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG        ac4c578f6020d5164f66ccb26b4727dea657a12b # master 1/1/2023
    SYSTEM
)

FetchContent_MakeAvailable(json inja argparse)