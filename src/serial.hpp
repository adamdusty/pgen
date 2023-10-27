#pragma once

#include <filesystem>
#include <istream>
#include <vector>

#include "point.hpp"

namespace pgen {

namespace fs = std::filesystem;

auto deserialize_directory(const fs::path& root) -> std::vector<point>;
auto deserialize_json(std::istream& input) -> std::vector<point>;

} // namespace pgen