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
            point.path         = fs::relative(entry.path(), root);
            point.is_directory = true;
        }

        if(entry.is_regular_file()) {
            point.path = fs::relative(entry.path(), root);

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

auto deserialize_json(std::istream& input) -> project {
    auto proj = pgen::project{};

    auto data = json::parse(input);

    if(data.contains("vars")) {
        for(const auto& var: data.at("vars")) {
            proj.vars.emplace_back(var);
        }
    }

    if(!data.contains("points")) {
        return {};
    }

    for(const auto& element: data.at("points")) {
        auto content = std::string{};
        if(element.contains("content")) {
            content = element.at("content");
        }
        proj.points.emplace_back(element.at("path"), content, element.at("dir"));
    }

    return proj;
}

auto deserialize_json(const fs::path& path) -> project {
    auto stream = std::ifstream{path};
    return deserialize_json(stream);
}

auto to_json(const project& proj) -> std::string {
    auto data = json{};

    for(const auto& var: proj.vars) {
        data["vars"].emplace_back(var);
    }

    for(const auto& point: proj.points) {
        json point_json;
        point_json["path"]    = point.path;
        point_json["content"] = point.content;
        point_json["dir"]     = point.is_directory;
        data["points"].emplace_back(point_json);
    }

    return data.dump(4);
}

} // namespace pgen