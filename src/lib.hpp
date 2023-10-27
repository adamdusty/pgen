#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "point.hpp"

namespace pgen {

namespace fs = std::filesystem;
using std::optional;
using std::string;
using std::unordered_map;
using std::vector;

struct result {
    bool success;
    string message;

    explicit operator bool() const { return success; }
};

auto render(const unordered_map<string, string>& map, const vector<point>& points) -> vector<point>;
auto write_points(const fs::path& root, const vector<point>& points) -> result;

} // namespace pgen