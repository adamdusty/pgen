#include "lib.hpp"

#include "render.hpp"
#include <format>
#include <fstream>
#include <iostream>
#include <unordered_map>

#include <iterator>
#include <ranges>

namespace pgen {

template<typename T, typename E>
struct to_expected_vector_fn : std::ranges::range_adaptor_closure<to_expected_vector_fn<T, E>> {
    template<std::input_iterator I, std::sentinel_for<I> S>
    auto operator()(I first, S last) const -> std::expected<std::vector<T>, E> {
        auto vec = std::vector<T>{};

        while(first != last) {
            if(*first) {
                vec.emplace_back(**first);
                ++first;
            } else {
                return std::unexpected((*first).error());
            }
        }

        return vec;
    }

    template<std::ranges::input_range R>
    auto operator()(R&& rng) const -> std::expected<std::vector<T>, E> {
        return operator()(std::ranges::begin(rng), std::ranges::end(rng));
    }
};

template<typename T, typename E>
auto to_expected_vector() {
    return to_expected_vector_fn<T, E>();
}

namespace fs = std::filesystem;

auto render_template(const project_template& tmpl, const std::unordered_map<std::string, std::string>& defs)
    -> std::expected<rendered_template, std::string> {

    // clang-format off
    auto rendered_directories_result = tmpl.directories
                                       | std::views::transform([&](auto d) { return render(d, defs); }) 
                                       | to_expected_vector<fs::path, std::string>();
    // clang-format on

    if(!rendered_directories_result) {
        return std::unexpected(rendered_directories_result.error());
    }

    auto files = std::unordered_map<fs::path, std::string>();
    for(const auto& project_file: tmpl.files) {
        auto path_result = render(project_file.path, defs);
        if(!path_result) {
            return std::unexpected(path_result.error());
        }

        auto content = std::string();
        if(project_file.content) {
            auto content_result = render(*project_file.content, defs);
            if(!content_result) {
                return std::unexpected(content_result.error());
            }

            content = *content_result;
        }

        files[*path_result] = content;
    }

    return rendered_template{.directories = *rendered_directories_result, .files = files};
}

auto write_template(const std::filesystem::path& destination,
                    const rendered_template& tmpl) -> std::expected<void, std::string> {

    if(!fs::create_directory(destination)) {
        return std::unexpected(std::format("Unable to create directory at {}", destination.string()));
    }

    for(const auto& dir: tmpl.directories) {
        auto path = destination / dir;

        if(!fs::create_directories(path)) {
            std::cerr << std::format("Error creating directory: {}\n", dir.string());
        }
    }

    for(const auto& [fpath, content]: tmpl.files) {
        auto path = destination / fpath;

        fs::create_directories(path.parent_path());

        auto file = std::ofstream(path);

        if(!file) {
            std::cerr << std::format("Error creating file: {}\n", path.string());
            continue;
        }

        file << content;
    }

    return {};
}

auto from_directory(const std::filesystem::path& root,
                    const std::vector<templated_variable>& vars) -> std::expected<project_template, std::string> {
    auto directories = std::vector<std::string>();
    auto files       = std::vector<project_file>();

    for(const auto& entry: fs::recursive_directory_iterator(root)) {
        if(entry.is_directory()) {
            directories.emplace_back(entry.path().string());
        } else {
            files.emplace_back(project_file::from_path(entry));
        }
    }

    return project_template{vars, directories, files};
}

} // namespace pgen