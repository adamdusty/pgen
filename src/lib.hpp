#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

namespace pgen {

namespace fs = std::filesystem;

auto read_template(const std::string& templ_str) -> std::unordered_map<fs::path, std::string>;
auto render_content(const std::unordered_map<fs::path, std::string> files,
                    const std::unordered_map<std::string, std::string> values)
    -> std::unordered_map<fs::path, std::string>;
auto write_files(const fs::path root, const std::unordered_map<fs::path, std::string> files) -> void;

} // namespace pgen