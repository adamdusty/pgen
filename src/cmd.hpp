#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>

namespace pgen {
namespace fs = std::filesystem;

struct generate {
    fs::path dest;
    fs::path tmpl;

    auto execute() const -> int;
};

struct from_directory {
    fs::path dir;
    std::optional<fs::path> output;

    auto execute() const -> int;
};

} // namespace pgen