#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

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

struct point {
    enum class point_type {
        file,
        directory,
    };

    point_type type = point_type::file;
    fs::path path;
    optional<string> content;
};

auto render(const unordered_map<string, string>& map, const vector<point>& points) -> vector<point>;
auto write_points(const fs::path& root, const vector<point>& points) -> result;

} // namespace pgen