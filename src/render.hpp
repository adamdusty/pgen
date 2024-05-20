#pragma once

#include <expected>
#include <string>
#include <string_view>
#include <unordered_map>

namespace pgen {

auto render(std::string_view str, const std::unordered_map<std::string, std::string>& vars)
    -> std::expected<std::string, std::string>;

} // namespace pgen