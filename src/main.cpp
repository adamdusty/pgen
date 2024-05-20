#include <argparse/argparse.hpp>
#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <yaml-cpp/yaml.h>

#include "cli.hpp"
#include "lib.hpp"

namespace ap = argparse;
namespace fs = std::filesystem;

auto parse_defs_file(const fs::path& defs_path) -> std::unordered_map<std::string, std::string>;
auto prompt_for_variable_defs(const pgen::project_template& tmpl) -> std::unordered_map<std::string, std::string>;

auto main(int argc, char* argv[]) -> int {
    auto version = std::string("0.1.0");
    auto parser  = ap::ArgumentParser("pgen", version);

    auto gen_cmd = ap::ArgumentParser("gen", version, ap::default_arguments::help);
    gen_cmd.add_description("Generate project");
    gen_cmd.add_argument("destination").help("Path to project generation destination");
    gen_cmd.add_argument("--template").help("Path to project template file").required();
    gen_cmd.add_argument("--definitions").help("Path to definitions file");

    auto fd_cmd = ap::ArgumentParser("fd", version, ap::default_arguments::help);
    fd_cmd.add_description("Generate project template from a directory");
    fd_cmd.add_argument("root").help("Root directory to generate template from");
    fd_cmd.add_argument("-o", "--output").help("File to write template to");
    fd_cmd.add_argument("-i", "--interactive")
        .help("Interactively prompt for variable definitions")
        .default_value(false)
        .implicit_value(true);

    parser.add_subparser(gen_cmd);
    parser.add_subparser(fd_cmd);

    try {
        parser.parse_args(argc, argv);
    } catch(const std::exception& err) {
        std::cerr << err.what() << '\n';
        std::cerr << parser << '\n';
        return 1;
    }

    if(parser.is_subcommand_used(gen_cmd)) {
        auto destination_path_str = gen_cmd.get<std::string>("destination");
        auto template_path_str    = gen_cmd.get<std::string>("template");
        auto definitions_path_str = gen_cmd.present<std::string>("definitions");

        auto tmpl_res = pgen::project_template::from_file(template_path_str);
        if(!tmpl_res) {
            std::cerr << tmpl_res.error() << '\n';
            return 1;
        }

        auto defs = std::unordered_map<std::string, std::string>();
        if(auto& defs_path = definitions_path_str) {
            defs = parse_defs_file(*defs_path);
        } else {
            defs = prompt_for_variable_defs(*tmpl_res);
        }

        auto render_result = pgen::render_template(*tmpl_res, defs);
        if(!render_result) {
            std::cerr << render_result.error() << '\n';
            return 1;
        }

        auto write_result = pgen::write_template(destination_path_str, *render_result);
        if(!write_result) {
            std::cerr << write_result.error() << '\n';
            return 1;
        }

    } else if(parser.is_subcommand_used(fd_cmd)) {
        // Generate template from directory
        auto root_str   = fd_cmd.get<std::string>("root");
        auto output_str = fd_cmd.get<std::string>("output");

        auto vars = std::vector<pgen::templated_variable>();

        auto done = false;
        while(fd_cmd.get<bool>("interactive")) {
            auto ident = pgen::prompt(std::cout, "Variable identifier(Leave empty to finish)");
            if(ident.empty()) {
                done = true;
                break;
            }

            auto display       = pgen::prompt(std::cout, "Variable display name");
            auto desc          = pgen::prompt(std::cout, "Variable description");
            auto default_value = pgen::prompt(std::cout, "Variable default value");

            auto var       = pgen::templated_variable();
            var.identifier = ident;
            if(display.empty()) {
                var.display_name = std::nullopt;
            } else {
                var.display_name = display;
            }

            if(desc.empty()) {
                var.description = std::nullopt;
            } else {
                var.description = desc;
            }

            if(default_value.empty()) {
                var.default_value = std::nullopt;
            } else {
                var.default_value = default_value;
            }

            vars.emplace_back(var);
            std::cerr << "\nAdded variable:\n"
                      << std::format("Identifier: {}\nDisplay Name: {}\nDescription: {}\nDefault: {}\n\n",
                                     var.identifier,
                                     var.display_name ? *var.display_name : "",
                                     var.description ? *var.description : "",
                                     var.default_value ? *var.default_value : "");
        }

        auto fd_result = pgen::from_directory(root_str, vars);
        if(!fd_result) {
            std::cerr << "Error: " << fd_result.error() << '\n';
        }
    } else {
        std::cerr << parser;
    }

    return 0;
}

auto parse_defs_file(const fs::path& defs_path) -> std::unordered_map<std::string, std::string> {
    auto file_stream = std::ifstream(defs_path);
    auto defs_stream = std::stringstream();
    defs_stream << file_stream.rdbuf();

    auto defs_str = defs_stream.str();
    auto yaml     = YAML::Load(defs_str);

    auto defs = std::unordered_map<std::string, std::string>();
    for(const auto& node: yaml) {
        defs[node.first.as<std::string>()] = node.second.as<std::string>();
    }

    return defs;
}

auto prompt_for_variable_defs(const pgen::project_template& tmpl) -> std::unordered_map<std::string, std::string> {

    auto defs = std::unordered_map<std::string, std::string>();

    for(const auto& var: tmpl.variables) {
        auto resp            = pgen::prompt(std::cout,
                                 var.display_name ? *var.display_name : var.identifier,
                                 [](std::string_view s) { return s.size() > 0; });
        defs[var.identifier] = resp;

        // Starts to throw exceptions if the user send a SIGINT
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return defs;
}