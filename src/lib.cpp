#include "lib.hpp"

#include "inja.hpp"
#include <algorithm>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fstream>
#include <numeric>
#include <optional>
#include <ranges>
#include <regex>
#include <string>
#include <toml++/toml.h>
#include <vector>

namespace pgen {

struct validation_result {
    bool valid;
    std::string msg;
};

auto validate_template(const toml::table& tbl) -> validation_result {
    // Validate vars
    if(tbl.find("vars") != tbl.end()) {
        if(const auto* vars = tbl.at("vars").as_array()) {
            for(const auto& var: *vars) {
                if(!var.value<std::string>()) {
                    // Variable value can't be cast to a string
                    return {false, "Variable value cannot be cast to a string"};
                }
            }
        } else {
            // Vars entry can't be cast to an array
            return {false, "Vars entry can't be cast to an array"};
        }
    }

    // Validate files
    if(tbl.find("files") == tbl.end()) {
        // Template doesn't have a files array (nothing to generate)
        return {false, "Template doesn't contain a `files` array (nothing to generate)."};
    }

    if(const auto* files = tbl.at("files").as_array()) {
        for(const auto& file: *files) {
            if(const auto* file_data = file.as_table()) {
                if(file_data->find("path") == file_data->end()) {
                    // File doesn't have a path entry
                    return {false, "File entry doesn't declare a path."};
                }
                if(file_data->find("content") == file_data->end()) {
                    // File doesn't have a content entry
                    return {false, "File entry doesn't declare any content."};
                }

                if(!file_data->at("path").value<std::string>()) {
                    // File data path entry can't be cast to string
                    return {false, "File path can't be cast to a string."};
                }
                if(!file_data->at("content").value<std::string>()) {
                    // File data content entry can't be cast to string
                    return {false, "File content can't be cast to a string."};
                }
            } else {
                // File entry can't be cast to table
                return {false, "File entry isn't a table."};
            }
        }
    } else {
        // Files entry can't be cast to an array
        return {false, "`files` is not an array."};
    }

    // Validate pregen commands
    if(tbl.find("pregen_commands") != tbl.end()) {
        if(const auto* pregen = tbl.at("pregen_commands").as_array()) {
            for(const auto& cmd: *pregen) {
                if(!cmd.value<std::string>()) {
                    // Command is not a string
                    return {false, "Pregen command can't be cast to a string."};
                }
            }
        } else {
            // Pregen commands cannot be cast to array
            return {false, "Pregen commands is not an array."};
        }
    }

    // Validate postgen commands
    if(tbl.find("postgen_commands") != tbl.end()) {
        if(const auto* postgen = tbl.at("postgen_commands").as_array()) {
            for(const auto& cmd: *postgen) {
                if(!cmd.value<std::string>()) {
                    // Command is not a string
                    return {false, "Postgen command can't be cast to a string."};
                }
            }
        } else {
            // Postgen commands cannot be cast to array
            return {false, "Postgen commands is not an array."};
        }
    }

    // No validation issues
    return {true};
}

auto read_template(std::istream& templ_str) -> std::optional<project_template> {

    toml::table table;

    try {
        table = toml::parse(templ_str);
    } catch(toml::parse_error& err) {
        fmt::println("Error parsing template: {}", err.what());
        return std::nullopt;
    }

    auto validation = validate_template(table);
    if(!validation.valid) {
        fmt::println("Template invalid: {}", validation.msg);
        return std::nullopt;
    }

    auto templ = project_template{};

    if(table.find("vars") != table.end()) {
        const auto* vars = table.at("vars").as_array();
        for(const auto& var: *vars) {
            auto value = var.value<std::string>();
            if(value) {
                templ.vars.emplace_back(*value);
            }
        }
    }

    for(auto& file_entry: *table.at("files").as_array()) {
        const auto* file = file_entry.as_table();
        auto path        = file->at("path").value<std::string>();
        auto content     = file->at("content").value<std::string>();
        if(!path) {
            fmt::println("Error getting path from file object. Value cannot be cast to a string.");
            return std::nullopt;
        }

        if(!content) {
            fmt::println("Error getting content from file object with path ({}). Value cannot be cast to a string.",
                         *path);
            return std::nullopt;
        }

        templ.files.emplace(*path, *content);
    }

    if(table.find("pregen_commands") != table.end()) {
        for(auto& cmd: *table.at("pregen_commands").as_array()) {
            if(auto cmd_value = cmd.value<std::string>()) {
                templ.pregen_commands.emplace_back(*cmd_value);
            }
        }
    }

    if(table.find("postgen_commands") != table.end()) {
        for(auto& cmd: *table.at("postgen_commands").as_array()) {
            if(auto cmd_value = cmd.value<std::string>()) {
                templ.postgen_commands.emplace_back(*cmd_value);
            }
        }
    }

    return templ;
}

auto render_content(const std::unordered_map<fs::path, std::string>& files,
                    const std::unordered_map<std::string, std::string>& values)
    -> std::unordered_map<fs::path, std::string> {

    auto env = inja::Environment{};
    env.set_expression("{*", "*}");

    auto json_defs = inja::json{};
    for(const auto& [k, v]: values) {
        json_defs.emplace(k, v);
    }

    auto rendered = std::unordered_map<fs::path, std::string>{};

    for(const auto& [file, content]: files) {
        auto rendered_file    = env.render(file.string(), json_defs);
        auto rendered_content = env.render(content, json_defs);
        rendered.emplace(rendered_file, rendered_content);
    }

    return rendered;
}

auto write_files(const fs::path& root, const std::unordered_map<fs::path, std::string>& files) -> write_result {
    if(fs::exists(root)) {
        return {false, "Root path already exists."};
    }

    if(!fs::create_directory(root)) {
        return {false, "Unable to create root directory"};
    }

    for(const auto& [relpath, content]: files) {
        if(!fs::exists(root / relpath.parent_path())) {
            auto created = fs::create_directories(root / relpath.parent_path());
            if(!created) {
                return {false, fmt::format("Unable to create directory: {}", relpath.parent_path().string())};
            }
        }

        auto path = root / relpath;
        auto file = std::ofstream{path};
        file << content;
        file.close();
        fmt::println("File written: {}", path.string());
    }

    return {true, "Files written successfully"};
}

} // namespace pgen