#include "cli.hpp"

namespace pgen {

auto prompt(std::ostream& to, std::string_view msg, validator validator) -> std::string {
    auto response = std::string();

    to << msg << ": ";
    std::getline(std::cin, response);

    if(validator == nullptr || validator(response)) {
        return response;
    }

    while(!validator(response)) {
        to << "Response invalid\n";
        to << msg << ": ";
        std::getline(std::cin, response);
    }

    return response;
}

} // namespace pgen