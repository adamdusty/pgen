#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace pgen {

namespace fs = std::filesystem;
using std::string;

struct point {
    fs::path path;
    string content;
    bool is_directory;

    auto operator<=>(const point&) const = default;
};

struct project {
    std::vector<string> vars;
    std::vector<point> points;

    auto operator<=>(const project&) const = default;
};

} // namespace pgen