#include "lib.hpp"

#include <algorithm>
#include <fmt/format.h>
#include <fstream>
#include <regex>
#include <toml++/toml.h>

namespace pgen {

auto read_template(std::istream& templ_str) -> project_template {

    toml::table t;

    try {
        t = toml::parse(templ_str);
    } catch(toml::parse_error& err) {
        fmt::println("Error parsing template: {}", err.what());
        std::exit(1);
    }

    auto templ = project_template{};

    for(auto& v: *t.at("vars").as_array()) {
        auto value = v.value<std::string>();
        if(value) {
            templ.vars.emplace_back(*value);
        }
    }

    for(auto& [p, c]: *t.at("files").as_table()) {
        auto content = c.value<std::string>();
        if(content) {
            templ.files.emplace(p.data(), *content);
        }
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