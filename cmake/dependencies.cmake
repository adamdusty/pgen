include(FetchContent)
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG        10.1.1
    GIT_SHALLOW    TRUE
    SYSTEM
)

set(JSON_BuildTests OFF CACHE INTERNAL "Don't build tests")
# FetchContent_Declare(
#     json
#     GIT_REPOSITORY https://github.com/nlohmann/json.git
#     GIT_TAG        v3.11.2
#     GIT_SHALLOW    TRUE
#     SYSTEM
# )
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

FetchContent_MakeAvailable(fmt json inja)