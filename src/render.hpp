#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "point.hpp"

namespace pgen {

using std::string;
using std::unordered_map;
using std::vector;

auto render(const unordered_map<string, string>& map, const vector<point>& points) -> vector<point>;

} // namespace pgen