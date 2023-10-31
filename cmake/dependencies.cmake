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
FetchContent_Declare(
    inja
    GIT_REPOSITORY https://github.com/pantor/inja.git
    GIT_TAG        v3.4.0
    GIT_SHALLOW    TRUE
    SYSTEM
)

set(ARGPARSE_BUILD_TESTS OFF CACHE INTERNAL "Turn off building argparse tests")
set(ARGPARSE_BUILD_SAMPLES OFF CACHE INTERNAL "Turn off building argparse samples")
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG        e4d4f67a449881ba4970dde2bb18c9c5d28669a9 # master 10/31/2023
    GIT_SHALLOW    TRUE
    SYSTEM
)

FetchContent_MakeAvailable(json inja argparse)