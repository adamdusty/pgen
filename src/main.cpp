#include <filesystem>
#include <format>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include <argparse/argparse.hpp>

#include "project.hpp"
#include "render.hpp"
#include "serial.hpp"
#include "write.hpp"

namespace ap = argparse;
namespace fs = std::filesystem;

auto main(int argc, char* argv[]) -> int {

    auto cli = ap::ArgumentParser{"pgen", "0.1.0"};

    cli.add_argument("dest").help("Destination directory for project.");
    cli.add_argument("-t", "--template").help("Template file.").required();

    try {
        cli.parse_args(argc, argv);
    } catch(const std::exception& err) {
        std::cerr << "Error parsing arguments: " << err.what() << std::endl;
        std::cerr << cli;
        return 1;
    }

    auto dest_str = cli.get<std::string>("dest");
    auto tmpl_str = cli.get<std::string>("template");

    auto dest_path = fs::path{dest_str};
    auto tmpl_path = fs::path{tmpl_str};

    if(fs::exists(dest_path)) {
        std::cerr << std::format("Destination directory already exists: {}\n", dest_path.string());
        std::cerr << "Aborting...";
        return 1;
    }

    if(!fs::exists(tmpl_path)) {
        std::cerr << std::format("Cannot find template: {}\n", tmpl_path.string());
        std::cerr << "Aborting...";
        return 1;
    }

    // Read template
    auto tmpl = pgen::deserialize_json(tmpl_path);

    // Get vars from user
    auto user_vars = std::unordered_map<std::string, std::string>{};
    for(const auto& var: tmpl.vars) {
        auto input = std::string{};
        std::cout << std::format("{}: ", var);
        std::getline(std::cin, input);
        user_vars.emplace(var, input);
    }

    // Render points
    auto rendered = pgen::render(user_vars, tmpl.points);

    // Write points
    auto result = pgen::write_points(dest_path, rendered);

    return 0;
}