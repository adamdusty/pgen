#pragma once

#include <expected>
#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace pgen {

struct templated_variable {
    std::string identifier;
    std::optional<std::string> display_name;
    std::optional<std::string> description;
    std::optional<std::string> default_value;
};

struct project_file {
    std::string path;
    std::optional<std::string> content;

    static auto from_path(const std::filesystem::path& path) -> project_file;
};

struct project_template {
    std::vector<templated_variable> variables;
    std::vector<std::string> directories;
    std::vector<project_file> files;

    static auto from_file(const std::filesystem::path& path) -> std::expected<project_template, std::string>;
};

struct rendered_template {
    std::vector<std::filesystem::path> directories;
    std::unordered_map<std::filesystem::path, std::string> files;
};

auto parse_template(const std::string& tmpl_str) -> std::expected<project_template, std::string>;

} // namespace pgen