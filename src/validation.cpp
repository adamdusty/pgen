#include "validation.hpp"

#include <format>
#include <nlohmann/json.hpp>
#include <string>

namespace pgen {

using nlohmann::json;

auto validate_vars(const json& json) -> validation_result {
    auto result = validation_result{};
    if(!json.contains("vars")) {
        return {true, "No vars array."};
    }

    if(!json.at("vars").is_array()) {
        return {false, "`vars` is not an array."};
    }

    for(const auto& var: json.at("vars")) {
        if(!var.is_string()) {
            return {false, std::format("var `{}` is not a string.", var.dump())};
        }
    }

    return {true, ""};
}

auto validate_points(const json& json) -> validation_result {
    if(!json.contains("points")) {
        return {true, "No points"};
    }

    if(!json.at("points").is_array()) {
        return {false, "`points` is not an array."};
    }

    for(const auto& point: json.at("points")) {
        if(!point.is_object()) {
            return {false, "point is not an object"};
        }

        if(!(point.contains("content") && point.contains("dir") && point.contains("path"))) {
            return {false, std::format("point `{}` is doesn't define all fields.", point.dump())};
        }
    }

    return {true, ""};
}

auto validate_json(const json& data) -> validation_result {
    auto result = validation_result{false, ""};

    // validate vars
    result = validate_vars(data);
    if(!result.valid) {
        return result;
    }

    // validate points
    result = validate_points(data);
    if(!result.valid) {
        return result;
    }

    return {true, ""};
}

} // namespace pgen