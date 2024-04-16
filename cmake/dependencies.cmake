include(FetchContent)

FetchContent_Declare(
    expected-lite
    GIT_REPOSITORY https://github.com/martinmoene/expected-lite.git
    GIT_TAG        3634b0a6d8dffcffad4d1355253d79290c0c754c # master as of 4/16/2024
    GIT_SHALLOW    TRUE
    SYSTEM
)

FetchContent_MakeAvailable(expected-lite)