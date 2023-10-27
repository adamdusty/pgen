#include "render.hpp"

#include <inja/inja.hpp>
#include <nlohmann/json.hpp>

namespace pgen {

using nlohmann::json;

auto render(const unordered_map<string, string>& map, const vector<point>& points) -> std::vector<point> {
    auto vals = json{};
    for(const auto& [key, val]: map) {
        vals.emplace(key, val);
    }

    auto env = inja::Environment{};
    env.set_expression("{@", "@}");

    auto rendered_points = std::vector<point>{};

    for(const auto& point: points) {
        auto rendered         = pgen::point{};
        rendered.path         = env.render(point.path.string(), vals);
        rendered.is_directory = point.is_directory;

        if(!point.is_directory) {
            rendered.content = env.render(point.content, vals);
        }

        rendered_points.emplace_back(rendered);
    }

    return {rendered_points};
}

} // namespace pgen