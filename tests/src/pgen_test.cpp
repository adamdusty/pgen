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

    auto templ_toml =
        R"(vars = [ "app_name", "file_name" ]
            [[files]]
            path = "src/main.cpp"
            content = "hello world main"
            [[files]]
            path = "src/{{app_name}}.hpp"
            content = "hello world header"
            [[files]]
            path = "src/{{app_name}}.cpp"
            content = "hello world impl"
        )";
    auto templ_stream = std::stringstream{templ_toml};
    auto templ        = *pgen::read_template(templ_stream);

    CHECK_FALSE(templ.files.find("src/main.cpp") == templ.files.end());
    CHECK_FALSE(templ.files.find("src/{{app_name}}.hpp") == templ.files.end());
    CHECK_FALSE(templ.files.find("src/{{app_name}}.cpp") == templ.files.end());

    CHECK(templ.files.at("src/main.cpp") == "hello world main");
    CHECK(templ.files.at("src/{{app_name}}.hpp") == "hello world header");
    CHECK(templ.files.at("src/{{app_name}}.cpp") == "hello world impl");
    CHECK_FALSE(std::find(templ.vars.begin(), templ.vars.end(), "app_name") == templ.vars.end());
    CHECK_FALSE(std::find(templ.vars.begin(), templ.vars.end(), "file_name") == templ.vars.end());
}

TEST_CASE("Read template with non-string vars array", "[read_template]") {
    auto templ_toml = R"(
        vars = [1, 2, 3, "apple"]
        [[files]]
        path = "src/main.cpp"
        content = "hello world"
    )";

    auto templ_stream = std::stringstream{templ_toml};
    auto templ_opt    = pgen::read_template(templ_stream);

    CHECK_FALSE(templ_opt);
}

TEST_CASE("Read template with non-string files array", "[read_template]") {
    auto templ_toml = R"(
        vars = ["apple"]
        [[files]]
        path = 1
        content = "hello world"
    )";

    auto templ_stream = std::stringstream{templ_toml};
    auto templ_opt    = pgen::read_template(templ_stream);

    CHECK_FALSE(templ_opt);
}

TEST_CASE("Read template missing vars array", "[read_template]") {
    auto templ_toml = R"(
        [[files]]
        path = "src/main.cpp"
        content = "hello world"
    )";

    auto templ_stream = std::stringstream{templ_toml};
    auto templ_opt    = pgen::read_template(templ_stream);

    CHECK(templ_opt);
}

TEST_CASE("Read template files entry is not an array", "[read_template]") {
    auto templ_toml = R"(
        vars = ["one", "two"]
        [files]
        path = "src/main.cpp"
        content = "hello world"
    )";

    auto templ_stream = std::stringstream{templ_toml};
    auto templ_opt    = pgen::read_template(templ_stream);

    CHECK_FALSE(templ_opt);
}

TEST_CASE("Read template vars entry is not an array", "[read_template]") {
    auto templ_toml = R"(
        [vars]
        var1 = 1
        var2 = 2
        [files]
        path = "src/main.cpp"
        content = "hello world"
    )";

    auto templ_stream = std::stringstream{templ_toml};
    auto templ_opt    = pgen::read_template(templ_stream);

    CHECK_FALSE(templ_opt);
}

TEST_CASE("Read template with pregen commands", "[read_template]") {
    auto templ_toml = R"(
        vars = ["one", "two"]
        pregen_commands = ["ls .", "git init {{ directory }}"]
        
        [[files]]
        path = "src/main.cpp"
        content = "hello world"
    )";

    auto templ_stream = std::stringstream{templ_toml};
    auto templ_opt    = pgen::read_template(templ_stream);
    auto templ        = *templ_opt;

    CHECK_FALSE(std::find(templ.pregen_commands.begin(), templ.pregen_commands.end(), "ls .") ==
                templ.pregen_commands.end());
    CHECK_FALSE(std::find(templ.pregen_commands.begin(), templ.pregen_commands.end(), "git init {{ directory }}") ==
                templ.pregen_commands.end());
}

TEST_CASE("Read template with postgen commands", "[read_template]") {
    auto templ_toml = R"(
        vars = ["one", "two"]
        pregen_commands = ["ls .", "git init {{ directory }}"]
        postgen_commands = ["ls .", "git init {{ directory }}"]
        
        [[files]]
        path = "src/main.cpp"
        content = "hello world"
    )";

    auto templ_stream = std::stringstream{templ_toml};
    auto templ_opt    = pgen::read_template(templ_stream);
    auto templ        = *templ_opt;

    CHECK_FALSE(std::find(templ.postgen_commands.begin(), templ.postgen_commands.end(), "ls .") ==
                templ.postgen_commands.end());
    CHECK_FALSE(std::find(templ.postgen_commands.begin(), templ.postgen_commands.end(), "git init {{ directory }}") ==
                templ.postgen_commands.end());
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

    SECTION("Render upper and lower", "[render_content]") {
        files.emplace("src/{{file1_name}}.cpp", "hello {{upper(planet)}}");
        files.emplace("src/{{file2_name}}.cpp", "{{lower(greeting)}} world 2");
        values.emplace("file1_name", "file1");
        values.emplace("file2_name", "file2");
        values.emplace("planet", "world");
        values.emplace("greeting", "HELLO");

        auto rendered = pgen::render_content(files, values);

        CHECK_FALSE(rendered.find("src/file1.cpp") == rendered.end());
        CHECK_FALSE(rendered.find("src/file2.cpp") == rendered.end());
        CHECK(rendered.at("src/file1.cpp") == "hello WORLD");
        CHECK(rendered.at("src/file2.cpp") == "hello world 2");
    }

    SECTION("Render throws when variable not present", "[render_content]") {
        files.emplace("src/{{file1_name}}.cpp", "hello {{upper(planet)}}");
        files.emplace("src/{{file2_name}}.cpp", "{{lower(greeting)}} world 2");

        CHECK_THROWS(pgen::render_content(files, values));
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