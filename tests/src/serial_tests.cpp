#include "project.hpp"
#include "serial.hpp"
#include "utils.hpp"
#include <snitch/snitch.hpp>

#include <algorithm>
#include <filesystem>
#include <format>
#include <fstream>
#include <ranges>
#include <string>
#include <vector>

namespace fs = std::filesystem;
namespace rv = std::ranges;
using pgen::point;
using utils::random_string;

auto temp_dir() -> fs::path {
    return fs::temp_directory_path();
};

TEST_CASE("Directory empty", "[deserialize_directory]") {
    auto root = temp_dir() / "pgen" / "serialize" / "empty";
    fs::create_directories(root);

    auto actual = pgen::deserialize_directory(root);

    CHECK(actual.empty());

    fs::remove_all(root);
}

TEST_CASE("Directory, files, empty", "[deserialize_directory]") {
    auto root = temp_dir() / "pgen" / "serialize" / "empty_files";
    fs::create_directories(root);

    auto expected = std::vector<point>{};

    for(size_t i = 0; i < 10; ++i) {
        auto fname = random_string();
        auto file  = std::ofstream{root / fname};
        file.close();
        expected.emplace_back(fname);
    }

    auto actual = pgen::deserialize_directory(root);

    rv::sort(actual, [](point& a, point& b) { return a.path < b.path; });
    rv::sort(expected, [](point& a, point& b) { return a.path < b.path; });

    CHECK(actual == expected);

    fs::remove_all(root);
}

TEST_CASE("Directory, files, empty, content", "[deserialize_directory]") {
    auto root = temp_dir() / "pgen" / "serialize" / "mixed_files";
    fs::create_directories(root);

    auto expected = std::vector<point>{};

    for(size_t i = 0; i < 10; ++i) {
        auto fname = random_string();
        auto file  = std::ofstream{root / fname};
        file.close();
        expected.emplace_back(fname);
    }

    for(size_t i = 0; i < 10; ++i) {
        auto fname   = random_string();
        auto content = random_string(128);
        auto file    = std::ofstream{root / fname};
        file << content;
        file.close();
        expected.emplace_back(fname, content);
    }

    auto actual = pgen::deserialize_directory(root);

    rv::sort(actual, [](point& a, point& b) { return a.path < b.path; });
    rv::sort(expected, [](point& a, point& b) { return a.path < b.path; });

    CHECK(actual == expected);

    fs::remove_all(root);
}

TEST_CASE("Directory, subdirs, files, content", "[deserialize_directory]") {
    auto root = temp_dir() / "pgen" / "serialize" / "subdirs";
    fs::create_directories(root);

    auto expected = std::vector<point>{};

    for(size_t i = 0; i < 3; ++i) {
        auto subdir = root / std::format("subdir{}", i);
        fs::create_directories(subdir);
        expected.emplace_back(fs::relative(subdir, root), "", true);
        for(size_t i = 0; i < 3; ++i) {
            auto fname   = random_string();
            auto content = random_string(128);
            auto file    = std::ofstream{subdir / fname};
            file << content;
            file.close();
            expected.emplace_back(fs::relative(subdir, root) / fname, content);
        }
    }

    auto actual = pgen::deserialize_directory(root);

    rv::sort(actual, [](point& a, point& b) { return a.path < b.path; });
    rv::sort(expected, [](point& a, point& b) { return a.path < b.path; });

    CHECK(actual == expected);

    fs::remove_all(root);
}

TEST_CASE("Empty template", "[deserialize_json]") {
    auto data = std::stringstream{R"({})"};

    auto actual = *pgen::deserialize_json(data);

    CHECK(actual.vars.empty());
    CHECK(actual.points.empty());
}

TEST_CASE("Files, empty", "[deserialize_json]") {
    auto data = std::stringstream{R"(
        {
            "points":
                [
                    {
                        "path": "file1",
                        "content": "",
                        "dir": false
                    },
                    {
                        "path": "file2",
                        "content": "",
                        "dir": false
                    },
                    {
                        "path": "file3",
                        "content": "",
                        "dir": false
                    }
                ]
        }
    )"};

    auto expected = pgen::project{
        {},
        std::vector<point>{{
            {.path = "file1", .content = "", .is_directory = false},
            {.path = "file2", .content = "", .is_directory = false},
            {.path = "file3", .content = "", .is_directory = false},
        }},
    };

    auto actual = pgen::deserialize_json(data);

    rv::sort(actual->points, [](point& a, point& b) { return a.path < b.path; });
    rv::sort(expected.points, [](point& a, point& b) { return a.path < b.path; });

    CHECK(actual == expected);
}

TEST_CASE("Files, content", "[deserialize_json]") {
    auto data = std::stringstream{R"(
        {
            "points":
                [
                    {
                        "path": "file1",
                        "content": "hello mercury",
                        "dir": false
                    },
                    {
                        "path": "file2",
                        "content": "hello venus",
                        "dir": false
                    },
                    {
                        "path": "file3",
                        "content": "hello world",
                        "dir": false
                    }
                ]
        }
    )"};

    auto expected = pgen::project{
        {},
        std::vector<point>{{
            {.path = "file1", .content = "hello mercury", .is_directory = false},
            {.path = "file2", .content = "hello venus", .is_directory = false},
            {.path = "file3", .content = "hello world", .is_directory = false},
        }},
    };

    auto actual = pgen::deserialize_json(data);

    rv::sort(actual->points, [](point& a, point& b) { return a.path < b.path; });
    rv::sort(expected.points, [](point& a, point& b) { return a.path < b.path; });

    CHECK(actual == expected);
}

TEST_CASE("Directories, Files, empty, content", "[deserialize_json]") {
    auto data = std::stringstream{R"(
        {
            "points":
                [
                    {
                        "path": "subdir3",
                        "dir": true
                    },
                    {
                        "path": "subdir4",
                        "dir": true
                    },
                    {
                        "path": "subdir1/file1",
                        "content": "hello mercury",
                        "dir": false
                    },
                    {
                        "path": "subdir2/file2",
                        "content": "",
                        "dir": false
                    },
                    {
                        "path": "file3",
                        "content": "hello world",
                        "dir": false
                    }
                ]
        }
    )"};

    auto expected = pgen::project{
        {},
        std::vector<point>{{
            {.path = "subdir3", .content = "", .is_directory = true},
            {.path = "subdir4", .content = "", .is_directory = true},
            {.path = "subdir1/file1", .content = "hello mercury", .is_directory = false},
            {.path = "subdir2/file2", .content = "", .is_directory = false},
            {.path = "file3", .content = "hello world", .is_directory = false},
        }},
    };

    auto actual = pgen::deserialize_json(data);

    rv::sort(actual->points, [](point& a, point& b) { return a.path < b.path; });
    rv::sort(expected.points, [](point& a, point& b) { return a.path < b.path; });

    CHECK(actual == expected);
}

TEST_CASE("Directories, Files, empty, content, vars", "[deserialize_json]") {
    auto data = std::stringstream{R"(
        {
            "vars": [
                "project_name",
                "namespace"
            ],
            "points":
                [
                    {
                        "path": "subdir3",
                        "dir": true
                    },
                    {
                        "path": "subdir4",
                        "dir": true
                    },
                    {
                        "path": "subdir1/file1",
                        "content": "hello mercury",
                        "dir": false
                    },
                    {
                        "path": "subdir2/file2",
                        "content": "",
                        "dir": false
                    },
                    {
                        "path": "file3",
                        "content": "hello world",
                        "dir": false
                    }
                ]
        }
    )"};

    auto expected = pgen::project{
        {
            "project_name",
            "namespace",
        },
        std::vector<point>{{
            {.path = "subdir3", .content = "", .is_directory = true},
            {.path = "subdir4", .content = "", .is_directory = true},
            {.path = "subdir1/file1", .content = "hello mercury", .is_directory = false},
            {.path = "subdir2/file2", .content = "", .is_directory = false},
            {.path = "file3", .content = "hello world", .is_directory = false},
        }},
    };

    auto actual = pgen::deserialize_json(data);

    rv::sort(actual->points, [](point& a, point& b) { return a.path < b.path; });
    rv::sort(expected.points, [](point& a, point& b) { return a.path < b.path; });
    rv::sort(actual->vars);
    rv::sort(expected.vars);

    CHECK(actual == expected);
}