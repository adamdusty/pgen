#pragma once

#include <filesystem>
#include <istream>
#include <vector>

#include "project.hpp"

namespace pgen {

namespace fs = std::filesystem;

auto deserialize_directory(const fs::path& root) -> std::vector<point>;
auto deserialize_json(std::istream& input) -> project;
auto deserialize_json(const fs::path& path) -> project;

} // namespace pgen