#pragma once

#include "template.hpp"
#include <expected>
#include <filesystem>
#include <string>
#include <unordered_map>

namespace pgen {

auto render_template(const project_template& tmpl, const std::unordered_map<std::string, std::string>& defs)
    -> std::expected<rendered_template, std::string>;

auto write_template(const std::filesystem::path& destination,
                    const rendered_template& tmpl) -> std::expected<void, std::string>;

auto from_directory(const std::filesystem::path& root,
                    const std::vector<templated_variable>& vars) -> std::expected<project_template, std::string>;

} // namespace pgen