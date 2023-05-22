#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

namespace pgen {

namespace fs = std::filesystem;

auto render_content(const std::unordered_map<fs::path, std::string> files,
                    const std::unordered_map<std::string, std::string> values)
    -> std::unordered_map<fs::path, std::string>;
auto write_files(const std::unordered_map<fs::path, std::string> files) -> void;

} // namespace pgen