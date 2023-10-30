#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "project.hpp"

namespace pgen {

namespace fs = std::filesystem;
using std::string;
using std::vector;

struct write_result {
    bool success;
    string message;

    explicit operator bool() const { return success; }
};

auto write_points(const fs::path& root, const vector<point>& points) -> write_result;

} // namespace pgen