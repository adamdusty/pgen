set shell := ["pwsh.exe", "-c"]

configure:
    cmake --preset dev -G Ninja

build:
    cmake --build --preset dev

test:
    ctest --preset dev

testv:
    ctest --preset dev --verbose