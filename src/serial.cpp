#include "serial.hpp"

#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>

namespace pgen {

using nlohmann::json;

auto deserialize_directory(const fs::path& root) -> std::vector<point> {
    auto points = std::vector<point>{};

    for(const auto& entry: fs::recursive_directory_iterator(root)) {
        point point{};

        if(entry.is_directory()) {
            point.path         = entry.path();
            point.is_directory = true;
        }

        if(entry.is_regular_file()) {
            point.path = entry.path();

            if(entry.file_size() != 0) {
                auto stream  = std::ifstream{entry.path()};
                auto content = std::stringstream{};
                content << stream.rdbuf();
                point.content = content.str();
            }
        }

        points.emplace_back(point);
    }

    return points;
}

auto deserialize_json(std::istream& input) -> std::vector<point> {
    auto points = std::vector<point>{};

    auto data = json::parse(input);
    if(data.find("points") == data.end()) {
        return {};
    }

    for(const auto& element: data.at("points")) {
        auto content = std::string{};
        if(element.find("content") != element.end()) {
            content = element.at("content");
        }
        points.emplace_back(element.at("path"), content, element.at("dir"));
    }

    return points;
}

} // namespace pgen