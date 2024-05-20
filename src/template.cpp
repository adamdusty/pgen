#include "template.hpp"

#include <format>
#include <fstream>
#include <sstream>
#include <yaml-cpp/yaml.h>

namespace pgen {

namespace fs = std::filesystem;

auto project_file::from_path(const std::filesystem::path& path) -> project_file {
    if(fs::is_empty(path)) {
        return {path.string(), std::nullopt};
    }

    auto file_stream   = std::ifstream(path);
    auto string_stream = std::stringstream();
    string_stream << file_stream.rdbuf();

    return {fs::relative(path).string(), string_stream.str()};
}

auto project_template::from_file(const std::filesystem::path& path) -> std::expected<project_template, std::string> {
    auto template_file = std::ifstream(path);
    if(!template_file) {
        return std::unexpected("Template file could not be opened");
    }

    auto template_str = std::stringstream();
    template_str << template_file.rdbuf();

    return parse_template(template_str.str());
}

auto parse_variables(const YAML::Node& variables) -> std::expected<std::vector<templated_variable>, std::string> {
    if(!variables.IsSequence()) {
        return std::unexpected{"Variables entry not a sequence"};
    }

    auto vars = std::vector<templated_variable>{};

    for(const auto& var: variables) {
        if(!var["identifier"]) {
            return std::unexpected{"Variable entry doesn't define an identifier"};
        }

        auto templated_var       = templated_variable{};
        templated_var.identifier = var["identifier"].as<std::string>();

        if(auto display = var["display_name"]) {
            templated_var.display_name = display.as<std::string>();
        } else {
            templated_var.display_name = std::nullopt;
        }

        if(auto display = var["description"]) {
            templated_var.description = display.as<std::string>();
        } else {
            templated_var.description = std::nullopt;
        }

        if(auto display = var["default_value"]) {
            templated_var.default_value = display.as<std::string>();
        } else {
            templated_var.default_value = std::nullopt;
        }

        vars.emplace_back(templated_var);
    }

    return vars;
}

auto parse_directories(const YAML::Node& directories) -> std::expected<std::vector<std::string>, std::string> {
    if(!directories.IsSequence()) {
        return std::unexpected{"Directories entry is not a sequence"};
    }

    auto dirs = std::vector<std::string>{};

    for(const auto& dir: directories) {
        dirs.emplace_back(dir.as<std::string>());
    }

    return dirs;
}

auto parse_files(const YAML::Node& files) -> std::expected<std::vector<project_file>, std::string> {
    if(!files.IsSequence()) {
        return std::unexpected("Files entry is not a sequence");
    }

    auto project_files = std::vector<project_file>{};

    for(const auto& file: files) {
        if(!file["path"]) {
            return std::unexpected("File entry doesn't define a path");
        }

        auto f = project_file{};
        f.path = file["path"].as<std::string>();

        if(auto content = file["content"]) {
            f.content = content.as<std::string>();
        } else {
            f.content = std::nullopt;
        }

        project_files.emplace_back(f);
    }

    return project_files;
}

auto parse_template(const std::string& tmpl_str) -> std::expected<project_template, std::string> {
    auto yaml = YAML::Load(tmpl_str);

    auto tmpl = project_template{
        .variables   = {},
        .directories = {},
        .files       = {},
    };

    if(yaml["variables"]) {
        if(auto vars = parse_variables(yaml["variables"])) {
            tmpl.variables = *vars;
        } else {
            return std::unexpected(vars.error());
        }
    }

    if(yaml["directories"]) {
        if(auto dirs = parse_directories(yaml["directories"])) {
            tmpl.directories = *dirs;
        } else {
            return std::unexpected(dirs.error());
        }
    }

    if(yaml["files"]) {
        if(auto files = parse_files(yaml["files"])) {
            tmpl.files = *files;
        } else {
            return std::unexpected(files.error());
        }
    }

    return tmpl;
}

auto to_yaml(const project_template& tmpl) -> std::expected<std::string, std::string> {
    auto emitter = YAML::Emitter();
    emitter << YAML::BeginMap;

    if(!tmpl.variables.empty()) {
        emitter << YAML::Key << "variables";
        emitter << YAML::Value << YAML::BeginSeq;

        for(const auto& var: tmpl.variables) {
            emitter << YAML::BeginMap;
            emitter << YAML::Key << "identifier" << YAML::Value << var.identifier;

            if(var.display_name) {
                emitter << YAML::Key << "display_name" << YAML::Value << *var.display_name;
            }

            if(var.description) {
                emitter << YAML::Key << "description" << YAML::Value << *var.description;
            }

            if(var.default_value) {
                emitter << YAML::Key << "default_value" << YAML::Value << *var.default_value;
            }
            emitter << YAML::EndMap;
        }

        emitter << YAML::EndSeq;
        emitter << YAML::EndMap;
    }
    if(!tmpl.directories.empty()) {
        emitter << YAML::Key << "directories";
        emitter << YAML::Value << YAML::BeginSeq;

        for(const auto& dir: tmpl.directories) {
            emitter << dir;
        }

        emitter << YAML::EndSeq;
    }

    if(!tmpl.files.empty()) {
        emitter << YAML::Key << "files";
        emitter << YAML::Value << YAML::BeginSeq;

        for(const auto& file: tmpl.files) {
            emitter << YAML::BeginMap;
            emitter << YAML::Key << "path" << YAML::Value << file.path;

            if(file.content) {
                emitter << YAML::Key << "content" << YAML::Literal << YAML::Value << *file.content;
            }
            emitter << YAML::EndMap;
        }

        emitter << YAML::EndSeq;
    }

    emitter << YAML::EndMap;
    if(!emitter.good()) {
        return std::unexpected(std::format("Error converting to yaml: {}", emitter.GetLastError()));
    }

    return emitter.c_str();
}

} // namespace pgen