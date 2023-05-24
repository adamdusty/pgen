#include "lib.hpp"

#include "json.hpp"
#include <algorithm>
#include <fmt/format.h>
#include <fstream>
#include <regex>

namespace pgen {

using json = nlohmann::json;

auto read_template(std::istream& templ_str) -> project_template {
    // TODO: Implement

    auto j     = json::parse(templ_str);
    auto templ = project_template{};

    for(auto& v: j.at("vars")) {
        templ.vars.emplace_back(v);
    }

    for(auto& [path, content]: j.at("files").items()) {
        templ.files.emplace(path, content);
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

auto write_files(const fs::path root, const std::unordered_map<fs::path, std::string> files) -> void {
    // TODO: Implement

    if(fs::exists(root)) {
        return;
    }

    fs::create_directory(root);

    for(auto& [relpath, content]: files) {
        if(!fs::exists(root / relpath.parent_path())) {
            fs::create_directory(root / relpath.parent_path());
        }

        auto path = root / relpath;
        auto file = std::ofstream{path};
        file << content;
        file.close();
    }
}

} // namespace pgen