#include "render.hpp"
#include <snitch/snitch.hpp>

#include <vector>

#include "point.hpp"

using pgen::point;

TEST_CASE("Empty points", "[render]") {
    auto points = std::vector<point>{{
        {"", "", false},
        {"", "", false},
        {"", "", false},
        {"", "", false},
    }};

    auto actual   = pgen::render({}, points);
    auto expected = std::vector<point>{{
        {"", "", false},
        {"", "", false},
        {"", "", false},
        {"", "", false},
    }};

    CHECK(actual == expected);
}

TEST_CASE("Points, no vars, no content", "[render]") {
    auto points = std::vector<point>{{
        {"file1", "", false},
        {"file2", "", false},
        {"file3", "", false},
        {"file4", "", false},
    }};

    auto actual   = pgen::render({}, points);
    auto expected = std::vector<point>{{
        {"file1", "", false},
        {"file2", "", false},
        {"file3", "", false},
        {"file4", "", false},
    }};

    CHECK(actual == expected);
}

TEST_CASE("Points, vars, no content", "[render]") {
    auto points = std::vector<point>{{
        {"{@ fname @}1", "", false},
        {"{@ fname @}2", "", false},
        {"{@ fname @}3", "", false},
        {"{@ fname @}4", "", false},
    }};

    auto actual   = pgen::render({{"fname", "file"}}, points);
    auto expected = std::vector<point>{{
        {"file1", "", false},
        {"file2", "", false},
        {"file3", "", false},
        {"file4", "", false},
    }};

    CHECK(actual == expected);
}

TEST_CASE("Points, vars, content", "[render]") {
    auto points = std::vector<point>{{
        {"{@ fname @}1", "{@ fdata @}1", false},
        {"{@ fname @}2", "{@ fdata @}2", false},
        {"{@ fname @}3", "{@ fdata @}3", false},
        {"{@ fname @}4", "{@ fdata @}4", false},
    }};

    auto actual   = pgen::render({{"fname", "file"}, {"fdata", "hello world"}}, points);
    auto expected = std::vector<point>{{
        {"file1", "hello world1", false},
        {"file2", "hello world2", false},
        {"file3", "hello world3", false},
        {"file4", "hello world4", false},
    }};

    CHECK(actual == expected);
}

TEST_CASE("Points, directories, vars, content", "[render]") {
    auto points = std::vector<point>{{
        {"{@ subdir @}1", "", true},
        {"{@ fname @}1", "{@ fdata @}1", false},
        {"{@ fname @}2", "{@ fdata @}2", false},
        {"{@ fname @}3", "{@ fdata @}3", false},
        {"{@ fname @}4", "{@ fdata @}4", false},
    }};

    auto actual   = pgen::render({{"fname", "file"}, {"fdata", "hello world"}, {"subdir", "subdir"}}, points);
    auto expected = std::vector<point>{{
        {"subdir1", "", true},
        {"file1", "hello world1", false},
        {"file2", "hello world2", false},
        {"file3", "hello world3", false},
        {"file4", "hello world4", false},
    }};

    CHECK(actual == expected);
}