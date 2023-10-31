#include "cmd.hpp"

#include <algorithm>
#include <format>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include "project.hpp"
#include "render.hpp"
#include "serial.hpp"
#include "write.hpp"

namespace pgen {

auto generate::execute() const -> int {
    if(!fs::exists(tmpl)) {
        std::cerr << std::format("Template file at `{}` does not exist.", tmpl.string());
        return 1;
    }

    auto proj = deserialize_json(tmpl);

    auto user_vars = std::unordered_map<std::string, std::string>{};
    for(const auto& var: proj.vars) {
        std::cout << std::format("{}: ", var);
        auto line = std::string{};
        std::getline(std::cin, line);
        user_vars.emplace(var, line);
    }

    auto rendered = render(user_vars, proj.points);
    auto res      = write_points(dest, rendered);

    if(!res) {
        std::cerr << std::format("Error writing project files: {}", res.message);
        return 1;
    }

    return 0;
}

auto from_directory::execute() const -> int {
    if(!fs::exists(dir)) {
        std::cerr << std::format("Directory `{}` does not exist.", dir.string());
        return 1;
    }

    auto proj   = project{};
    proj.points = deserialize_directory(dir);

    std::cout << "Enter user variables (or press enter to continue):\n";
    auto quit = false;
    while(!quit) {
        auto line = std::string{};
        std::getline(std::cin, line);
        if(line.empty()) {
            quit = true;
        } else {
            proj.vars.emplace_back(line);
            std::cout << std::format("Variable `{}` added.\n", line);
        }
    }

    auto json = to_json(proj);

    if(output) {
        auto out = std::ofstream{*output};
        out << json;
    } else {
        std::cout << json;
    }

    return 0;
}

} // namespace pgen