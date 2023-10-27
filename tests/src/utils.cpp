#include "utils.hpp"

#include <algorithm>
#include <random>

namespace utils {

auto random_string(size_t size) -> std::string {
    auto base = std::string{"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"};

    auto rd   = std::random_device{};
    auto gen  = std::mt19937{rd()};
    auto dist = std::uniform_int_distribution<size_t>{0, base.size() - 1};

    auto res = std::string(size, '\0');
    for(size_t i = 0; i < size; ++i) {
        auto idx  = dist(gen);
        res.at(i) = base.at(idx);
    }

    return res;
}

} // namespace utils