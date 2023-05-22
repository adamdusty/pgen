#include "lib.hpp"
#include <filesystem>
#include <fmt/format.h>
#include <iostream>
#include <unordered_map>

namespace fs = std::filesystem;

auto main(int argc, char** argv) -> int {

    auto files  = std::unordered_map<fs::path, std::string>{};
    auto values = std::unordered_map<std::string, std::string>{};

    files.emplace("src/{{dir1}}/{{file1_name}}.cpp", "hello world 1");

    values.emplace("file1_name", "file1");
    values.emplace("dir1", "subdir");

    auto rendered = pgen::render_content(files, values);

    fmt::print("Rendered: {}\n", rendered.size());

    for(auto [path, content]: rendered) {
        fmt::print("PATH: {}\n", path.string());
    }

    return 0;
}