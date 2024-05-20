#pragma once

#include <expected>
#include <filesystem>
#include <iostream>
#include <optional>

namespace pgen {

namespace fs = std::filesystem;

struct gen {
    fs::path destination;
    fs::path template_path;
    std::optional<fs::path> definitions;
};

struct fd {
    fs::path root;
    std::optional<fs::path> variables;
    bool interactive;
};

using validator = bool (*)(std::string_view);

auto prompt(std::ostream& to, std::string_view msg, validator validator = nullptr) -> std::string;

} // namespace pgen