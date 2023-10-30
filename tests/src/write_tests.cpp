#include "project.hpp"
#include "write.hpp"
#include <snitch/snitch.hpp>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>

namespace fs = std::filesystem;

using pgen::point;
using std::vector;

// TODO:
//  - Test directory creation

TEST_CASE("Write points writes single file, relative path, empty content", "[write_points]") {
    auto tempdir = fs::temp_directory_path();
    auto root    = tempdir / "pgen_test";

    auto point_path = fs::path{"testfile"};
    auto points     = vector<point>{{.path = point_path}};
    auto result     = pgen::write_points(root, points);

    CHECK(result.success);

    CHECK(fs::exists(root / point_path));
    CHECK(fs::is_empty(root / point_path));

    fs::remove_all(root);
}

TEST_CASE("Write points writes multiple files, relative path, empty content", "[write_points]") {
    auto tempdir = fs::temp_directory_path();
    auto root    = tempdir / "pgen_test";

    auto paths  = vector<fs::path>{"testfile1", "testfile2", "testfile3"};
    auto points = vector<point>{
        {.path = "testfile1"},
        {.path = "testfile2"},
        {.path = "testfile3"},
    };
    auto result = pgen::write_points(root, points);

    CHECK(result.success);

    for(const auto& point_path: paths) {
        CHECK(fs::exists(root / point_path));
        CHECK(fs::is_empty(root / point_path));
    }

    fs::remove_all(root);
}

TEST_CASE("Write points writes single file, relative path, populated content", "[write_points]") {
    auto tempdir = fs::temp_directory_path();
    auto root    = tempdir / "pgen_test";

    auto point_path = fs::path{"testfile"};
    auto points     = vector<point>{{.path = point_path, .content = "hello world"}};
    auto result     = pgen::write_points(root, points);

    CHECK(result.success);

    CHECK(fs::exists(root / point_path));

    auto content = std::ifstream{root / points.at(0).path};
    auto buffer  = std::stringstream{};

    buffer << content.rdbuf();
    content.close();

    CHECK(buffer.str() == points.at(0).content);

    fs::remove_all(root);
}

TEST_CASE("Write points writes multiple files, relative path, populated content", "[write_points]") {
    auto tempdir = fs::temp_directory_path();
    auto root    = tempdir / "pgen_test";

    auto points = vector<point>{
        {.path = "testfile1", .content = "hello world1"},
        {.path = "testfile2", .content = "hello world2"},
        {.path = "testfile3", .content = "hello world3"},
    };
    auto result = pgen::write_points(root, points);

    CHECK(result.success);

    for(const auto& point: points) {
        CHECK(fs::exists(root / point.path));

        auto content = std::ifstream{root / point.path};
        auto buffer  = std::stringstream{};

        buffer << content.rdbuf();
        content.close();

        CHECK(buffer.str() == point.content);
    }

    fs::remove_all(root);
}

TEST_CASE("Writes files in subdirectories of root", "[write_points]") {
    auto tempdir = fs::temp_directory_path();
    auto root    = tempdir / "pgen_test";

    auto points = vector<point>{
        {.path = "subdir1/testfile1", .content = "hello world1"},
        {.path = "subdir2/testfile2", .content = "hello world2"},
        {.path = "subdir3/subdir4/testfile3", .content = "hello world3"},
    };
    auto result = pgen::write_points(root, points);

    CHECK(result.success);

    for(const auto& point: points) {
        CHECK(fs::exists(root / point.path));

        auto content = std::ifstream{root / point.path};
        auto buffer  = std::stringstream{};

        buffer << content.rdbuf();
        content.close();

        CHECK(buffer.str() == point.content);
    }

    fs::remove_all(root);
}

TEST_CASE("Return unsuccessful when root directory already exists", "[write_points]") {
    auto tempdir = fs::temp_directory_path();
    auto root    = tempdir / "pgen_test";
    fs::create_directories(root);

    auto points = vector<point>{
        {.path = "testfile1", .content = "hello world1"},
        {.path = "testfile2", .content = "hello world2"},
        {.path = "testfile3", .content = "hello world3"},
    };
    auto result = pgen::write_points(root, points);

    CHECK(!result);
    for(const auto& point: points) {
        CHECK(!fs::exists(root / point.path));
    }

    fs::remove_all(root);
}