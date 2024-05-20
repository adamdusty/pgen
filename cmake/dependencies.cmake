include(FetchContent)

FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG        76dc6715734295ff1866bfc32872ff2278258fc8 # master as of 4/16/2024
    GIT_SHALLOW    TRUE
    SYSTEM
)

FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG        ac4c2c2d96c974c6a4e487f288bfc89e108b64b1 # master as of 4/29/2024
    GIT_SHALLOW    TRUE
    SYSTEM
)

FetchContent_MakeAvailable(yaml-cpp argparse)