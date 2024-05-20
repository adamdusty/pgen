#include <argparse/argparse.hpp>
#include <chrono>
#include <filesystem>
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

    auto gen_cmd = ap::ArgumentParser("gen", version, argparse::default_arguments::help);
    gen_cmd.add_description("Generate project");
    gen_cmd.add_argument("destination").help("Path to project generation destination");
    gen_cmd.add_argument("--template").help("Path to project template file").required();
    gen_cmd.add_argument("--definitions").help("Path to definitions file");

    parser.add_subparser(gen_cmd);

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