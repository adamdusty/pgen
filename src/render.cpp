#include "render.hpp"

#include <cstddef>
#include <format>

namespace pgen {

auto render(std::string_view str,
            const std::unordered_map<std::string, std::string>& vars) -> std::expected<std::string, std::string> {
    std::size_t position = 0;

    auto sub    = std::string{};
    auto result = std::string{};

    while(position < str.size()) {
        if(str.at(position) == '{' && str.at(position + 1) == '@' && position + 2 < str.size()) {
            position += 2;

            // Consume whitespace after '{@'
            while(std::isspace(str.at(position)) != 0) {
                position += 1;

                if(position >= str.size()) {
                    return std::unexpected("Substitution left unclosed");
                }
            }

            // Read variable identifier
            while(str.at(position) != '@' && std::isspace(str.at(position)) == 0) {
                sub += str.at(position++);

                if(position >= str.size()) {
                    return std::unexpected("Substitution left unclosed");
                }
            }

            while(str.at(position) != '}') {
                position += 1;

                if(position >= str.size()) {
                    return std::unexpected("Substitution left unclosed");
                }
            }

            if(vars.contains(sub)) {
                result += vars.at(sub);
                sub.clear();

                position += 1; // Advance past closing brace
            } else {
                return std::unexpected(std::format("Template variable `{}` not defined", sub));
            }
        }

        if(position < str.size()) {
            result += str.at(position++);
        }
    }

    return result;
}

} // namespace pgen
