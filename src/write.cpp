#include "write.hpp"

#include <filesystem>
#include <format>
#include <fstream>
#include <sstream>

namespace pgen {

auto write_points(const fs::path& root, const std::vector<point>& points) -> write_result {
    if(fs::exists(root)) {
        return {
            .success = false,
            .message = std::format("Root path already exists: {}", root.string()),
        };
    }

    fs::create_directories(root);

    for(const auto& point: points) {
        if(point.is_directory && !fs::exists(root / point.path)) {
            fs::create_directories(root / point.path);
        }

        if(!fs::exists(root / point.path.parent_path())) {
            auto created = fs::create_directories(root / point.path.parent_path());
            if(!created) {
                return {
                    .success = false,
                    .message = std::format("Unable to create directory: {}", point.path.parent_path().string()),
                };
            }
        }

        auto path = root / point.path;
        auto file = std::ofstream{path};
        file << point.content;

        file.close();
    }

    return {
        .success = true,
        .message = "All points written successfully",
    };
}

} // namespace pgen