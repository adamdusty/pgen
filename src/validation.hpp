#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace pgen {

using nlohmann::json;
using std::string;

struct validation_result {
    bool valid;
    string message;
};

auto validate_json(const json& data) -> validation_result;

} // namespace pgen