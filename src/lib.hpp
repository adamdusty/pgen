#pragma once

#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace pgen {

namespace fs = std::filesystem;

struct project_template {
    std::vector<std::string> vars;
    std::unordered_map<fs::path, std::string> files;
    std::vector<std::string> pregen_commands;
    std::vector<std::string> postgen_commands;
};

struct write_result {
    bool success;
    std::string msg;
};

auto read_template(std::istream& templ_str) -> std::optional<project_template>;
auto render_content(const std::unordered_map<fs::path, std::string> files,
                    const std::unordered_map<std::string, std::string> values)
    -> std::unordered_map<fs::path, std::string>;
auto write_files(const fs::path root, const std::unordered_map<fs::path, std::string> files) -> write_result;

} // namespace pgen