#include "lib.hpp"
#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

namespace fs = std::filesystem;

TEST_CASE("Read template", "[read_template]") {

    auto templ_json =
        R"(vars = [ "app_name", "file_name" ]
            [files]
            "src/main.cpp" = "hello world main"
            "src/{{app_name}}.hpp" = "hello world header"
            "src/{{app_name}}.cpp" = "hello world impl"
        )";
    auto templ_stream = std::stringstream{templ_json};

    auto templ = pgen::read_template(templ_stream);

    CHECK_FALSE(templ.files.find("src/main.cpp") == templ.files.end());
    CHECK_FALSE(templ.files.find("src/{{app_name}}.hpp") == templ.files.end());
    CHECK_FALSE(templ.files.find("src/{{app_name}}.cpp") == templ.files.end());

    CHECK(templ.files.at("src/main.cpp") == "hello world main");
    CHECK(templ.files.at("src/{{app_name}}.hpp") == "hello world header");
    CHECK(templ.files.at("src/{{app_name}}.cpp") == "hello world impl");
    CHECK_FALSE(std::find(templ.vars.begin(), templ.vars.end(), "app_name") == templ.vars.end());
    CHECK_FALSE(std::find(templ.vars.begin(), templ.vars.end(), "file_name") == templ.vars.end());
}

TEST_CASE("Render Content", "[render_content]") {

    auto files  = std::unordered_map<fs::path, std::string>{};
    auto values = std::unordered_map<std::string, std::string>{};

    SECTION("Renders file name, no content substitution", "[render_content]") {
        files.emplace("src/{{file1_name}}.cpp", "hello world 1");
        files.emplace("src/{{file2_name}}.cpp", "hello world 2");
        files.emplace("src/{{ file3_name }}.cpp", "hello world 3");
        files.emplace("src/{{  file4_name  }}.cpp", "hello world 4");

        values.emplace("file1_name", "file1");
        values.emplace("file2_name", "file2");
        values.emplace("file3_name", "file3");
        values.emplace("file4_name", "file4");

        auto rendered = pgen::render_content(files, values);

        CHECK_FALSE(rendered.find("src/file1.cpp") == rendered.end());
        CHECK_FALSE(rendered.find("src/file2.cpp") == rendered.end());
        CHECK_FALSE(rendered.find("src/file3.cpp") == rendered.end());
        CHECK_FALSE(rendered.find("src/file4.cpp") == rendered.end());
    }

    SECTION("Multiple values in file name", "[render_content]") {
        files.emplace("src/{{dir1}}/{{file1_name}}.cpp", "hello world 1");

        values.emplace("file1_name", "file1");
        values.emplace("dir1", "subdir");

        auto rendered = pgen::render_content(files, values);

        CHECK_FALSE(rendered.find("src/subdir/file1.cpp") == rendered.end());
    }

    SECTION("Multiple values in content", "[render_content]") {
        files.emplace("src/file.cpp", "{{greeting}} {{ planet }}");

        values.emplace("greeting", "hello");
        values.emplace("planet", "world");

        auto rendered = pgen::render_content(files, values);

        CHECK_FALSE(rendered.find("src/file.cpp") == rendered.end());
        CHECK(rendered.at("src/file.cpp") == "hello world");
    }

    SECTION("Renders files & content", "[render_content]") {
        files.emplace("src/{{file1_name}}.cpp", "hello {{planet}}");
        files.emplace("src/{{file2_name}}.cpp", "{{greeting}} world 2");
        values.emplace("file1_name", "file1");
        values.emplace("file2_name", "file2");
        values.emplace("planet", "world");
        values.emplace("greeting", "hello");

        auto rendered = pgen::render_content(files, values);

        CHECK_FALSE(rendered.find("src/file1.cpp") == rendered.end());
        CHECK_FALSE(rendered.find("src/file2.cpp") == rendered.end());
        CHECK(rendered.at("src/file1.cpp") == "hello world");
        CHECK(rendered.at("src/file2.cpp") == "hello world 2");
    }
}

TEST_CASE("Writing files", "[write_files]") {
    auto files = std::unordered_map<fs::path, std::string>{};
    files.emplace("src/main.cpp", "hello from main");
    files.emplace("src/lib.hpp", "hello from lib header");
    files.emplace("src/lib.cpp", "hello from lib impl");

    auto temp = fs::temp_directory_path();
    auto root = temp / "pgen_test";
    auto res  = pgen::write_files(root, files);

    CHECK(res.success);

    for(auto& [p, c]: files) {
        CHECK(fs::exists(root / p));
        auto content_stream = std::ifstream{root / p};
        auto buffer         = std::stringstream{};
        buffer << content_stream.rdbuf();

        CHECK(buffer.str() == c);
    }

    fs::remove_all(root);
}