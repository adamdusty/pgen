#define TOML_IMPLEMENTATION

#include "lib.hpp"
#include <argparse/argparse.hpp>
#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

namespace fs = std::filesystem;
namespace ap = argparse;

auto main(int argc, char** argv) -> int {

    auto program = ap::ArgumentParser{"ProjectGen"};

    program.add_argument("dest").help("Destination directory for project.");
    program.add_argument("-t", "--template").help("Project template json file").required();

    try {
        program.parse_args(argc, argv);
    } catch(const std::runtime_error& err) {
        fmt::println("ERROR: {}", err.what());
        return 1;
    }

    auto dest          = fs::path{program.get<std::string>("dest")};
    auto template_path = fs::path{program.get<std::string>("--template")};

    if(fs::exists(dest)) {
        fmt::println("Destination directory already exists.");
        return 1;
    }

    if(!fs::exists(template_path)) {
        fmt::println("ERROR: Template file not found: {}", template_path.string());
        return 1;
    }

    auto template_file_stream = std::ifstream{template_path};
    auto templ_opt            = pgen::read_template(template_file_stream);
    if(!templ_opt) {
        fmt::println("Error reading template");
        return 1;
    }

    auto templ = *templ_opt;

    auto values = std::unordered_map<std::string, std::string>{};

    fmt::println("Enter values");
    for(auto& v: templ.vars) {
        auto line = std::string{};
        fmt::print("{}: ", v);
        std::getline(std::cin, line);
        values.emplace(v, line);
    }

    auto rendered = pgen::render_content(templ.files, values);
    auto result   = pgen::write_files(dest, rendered);

    fmt::println("{}", result.msg);

    return 0;
}