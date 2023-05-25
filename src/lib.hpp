#pragma once

#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace pgen {

namespace fs = std::filesystem;

struct project_template {
    std::vector<std::string> vars;
    std::unordered_map<fs::path, std::string> files;
};

struct write_result {
    bool success;
    std::string msg;
};

auto read_template(std::istream& templ_str) -> project_template;
auto render_content(const std::unordered_map<fs::path, std::string> files,
                    const std::unordered_map<std::string, std::string> values)
    -> std::unordered_map<fs::path, std::string>;
auto write_files(const fs::path root, const std::unordered_map<fs::path, std::string> files) -> write_result;

} // namespace pgen