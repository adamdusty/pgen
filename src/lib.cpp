#include "lib.hpp"

#include <algorithm>
#include <fmt/format.h>
#include <fstream>
#include <numeric>
#include <optional>
#include <regex>
#include <string>
#include <toml++/toml.h>
#include <vector>

namespace pgen {

auto validate_template(const toml::table tbl) -> bool {
    // Validate vars
    if(tbl.find("vars") != tbl.end()) {
        if(auto vars = tbl.at("vars").as_array()) {
            for(auto& v: *vars) {
                if(!v.value<std::string>()) {
                    // Variable value can't be cast to a string
                    return false;
                }
            }
        } else {
            // Vars entry can't be cast to an array
            return false;
        }
    }

    // Validate files
    if(tbl.find("files") == tbl.end()) {
        // Template doesn't have a files array (nothing to generate)
        return false;
    }

    if(auto files = tbl.at("files").as_array()) {
        for(auto& f: *files) {
            if(auto file_data = f.as_table()) {
                if(file_data->find("path") == file_data->end()) {
                    // File doesn't have a path entry
                    return false;
                }
                if(file_data->find("content") == file_data->end()) {
                    // File doesn't have a content entry
                    return false;
                }

                if(!file_data->at("path").value<std::string>()) {
                    // File data path entry can't be cast to string
                    return false;
                }
                if(!file_data->at("content").value<std::string>()) {
                    // File data content entry can't be cast to string
                    return false;
                }
            } else {
                // File entry can't be cast to table
                return false;
            }
        }
    } else {
        // Files entry can't be cast to an array
        return false;
    }

    // No validation issues
    return true;
}

auto read_template(std::istream& templ_str) -> std::optional<project_template> {

    toml::table t;

    try {
        t = toml::parse(templ_str);
    } catch(toml::parse_error& err) {
        fmt::println("Error parsing template: {}", err.what());
        return std::nullopt;
    }

    if(!validate_template(t)) {
        return std::nullopt;
    }

    auto templ = project_template{};

    if(t.find("vars") != t.end()) {
        auto vars = t.at("vars").as_array();
        for(auto& v: *vars) {
            auto value = v.value<std::string>();
            if(!value) {
                fmt::println("Error in vars array. Non-string var.");
                return std::nullopt;
            }

            templ.vars.emplace_back(*value);
        }
    }

    for(auto& f: *t.at("files").as_array()) {
        auto file    = f.as_table();
        auto path    = file->at("path").value<std::string>();
        auto content = file->at("content").value<std::string>();
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

    return templ;
}

auto render_content(const std::unordered_map<fs::path, std::string> files,
                    const std::unordered_map<std::string, std::string> values)
    -> std::unordered_map<fs::path, std::string> {

    auto rendered    = std::unordered_map<fs::path, std::string>{};
    auto match       = std::smatch{};
    auto ident_regex = std::regex{R"(\{\{\s*([_a-zA-Z][_a-zA-Z0-9]+)\s*\}\})", std::regex_constants::ECMAScript};

    for(auto& [file, content]: files) {
        auto file_path        = file.string();
        auto rendered_path    = file.string();
        auto rendered_content = content;

        while(std::regex_search(rendered_path, match, ident_regex)) {
            rendered_path = std::regex_replace(
                rendered_path, ident_regex, values.at(match[1].str()), std::regex_constants::format_first_only);
        }

        while(std::regex_search(rendered_content, match, ident_regex)) {
            rendered_content = std::regex_replace(
                rendered_content, ident_regex, values.at(match[1].str()), std::regex_constants::format_first_only);
        }

        rendered.emplace(rendered_path, rendered_content);
    }

    return rendered;
}

auto write_files(const fs::path root, const std::unordered_map<fs::path, std::string> files) -> write_result {
    if(fs::exists(root)) {
        return {false, "Root path already exists."};
    }

    if(!fs::create_directory(root)) {
        return {false, "Unable to create root directory"};
    }

    for(auto& [relpath, content]: files) {
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