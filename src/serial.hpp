#pragma once

#include <filesystem>
#include <istream>
#include <optional>
#include <string>
#include <vector>

#include "project.hpp"

namespace pgen {

namespace fs = std::filesystem;

auto deserialize_directory(const fs::path& root) -> std::vector<point>;
auto deserialize_json(std::istream& input) -> std::optional<project>;
auto deserialize_json(const fs::path& path) -> std::optional<project>;
auto to_json(const project& proj) -> std::string;

} // namespace pgen