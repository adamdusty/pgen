#pragma once

#include <filesystem>
#include <optional>
#include <string>

namespace pgen {

namespace fs = std::filesystem;
using std::string;

struct point {
    fs::path path;
    string content;
    bool is_directory;

    auto operator<=>(const point&) const = default;
};

} // namespace pgen