#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <lib.hpp>
#include <render.hpp>
#include <snitch/snitch.hpp>
#include <sstream>
#include <template.hpp>
#include <unordered_map>

namespace fs = std::filesystem;

template<std::ranges::range R, typename T>
    requires std::equality_comparable_with<T, std::ranges::range_value_t<R>>
auto contains(R& rng, T& value) -> bool {
    auto first = std::ranges::begin(rng);
    auto last  = std::ranges::end(rng);

    while(first != last) {
        if(*first == value) {
            return true;
        }
        ++first;
    }

    return false;
}

TEST_CASE("Parse template") {
    const auto* tmpl_str = R"(
        variables:
            - identifier: project_name
              display_name: Project Name
              description: Name of project
        directories:
            - docs
            - include
        files:
            - path: include/{@ project_name @}/{@ project_name @}.hpp
              content: |
                #pragma once

                #include <cstdint>
                
                namespace {@ project_name @} {
                
                constexpr std::uint32_t version = 1;
                
                }
            - path: src/main.cpp
              content: auto main() -> int { return 0; }
    )";

    auto tmpl = pgen::parse_template(tmpl_str);

    REQUIRE(tmpl);

    auto& var = tmpl->variables.at(0);
    CHECK(var.identifier == "project_name");
    CHECK(var.display_name.value() == "Project Name");
    CHECK(var.description.value() == "Name of project");
    CHECK_FALSE(var.default_value.has_value());

    auto& dirs = tmpl->directories;
    CHECK(dirs.at(0) == "docs");
    CHECK(dirs.at(1) == "include");

    auto& files = tmpl->files;
    CHECK(files.at(0).path == "include/{@ project_name @}/{@ project_name @}.hpp");
    CHECK(files.at(0).content == "#pragma once\n\n#include <cstdint>\n\nnamespace {@ project_name @} {\n\nconstexpr "
                                 "std::uint32_t version = 1;\n\n}\n");
    CHECK(files.at(1).path == "src/main.cpp");
    CHECK(files.at(1).content == "auto main() -> int { return 0; }");
}

TEST_CASE("Render", "[render]") {
    const auto* tmpl = "include/{@ project_name @}/{@project_name@}.hpp";

    auto vars = std::unordered_map<std::string, std::string>{
        {"project_name", "testproj"},
    };

    auto result = pgen::render(tmpl, vars);

    REQUIRE(result);
    CHECK(*result == "include/testproj/testproj.hpp");
}

TEST_CASE("Render", "[render]") {
    const auto* tmpl = "include/{@ project_name @}";

    auto vars = std::unordered_map<std::string, std::string>{
        {"project_name", "testproj"},
    };

    auto result = pgen::render(tmpl, vars);

    REQUIRE(result);
    CHECK(*result == "include/testproj");
}

TEST_CASE("Render", "[render]") {
    const auto* tmpl = R"(namespace {@ namespace @} {
          constexpr auto VERSION = 0;
      })";

    auto vars = std::unordered_map<std::string, std::string>{
        {"namespace", "nsp"},
    };

    auto result = pgen::render(tmpl, vars);

    REQUIRE(result);
    CHECK(*result == R"(namespace nsp {
          constexpr auto VERSION = 0;
      })");
}

TEST_CASE("Render returns unexpected when substitution left open", "[render]") {
    const auto* tmpl = "include/{@ project_name @}/{@project_name.hpp";

    auto vars = std::unordered_map<std::string, std::string>{
        {"project_name", "testproj"},
    };

    auto result = pgen::render(tmpl, vars);

    REQUIRE_FALSE(result);
    CHECK(result.error() == "Substitution left unclosed");
}

TEST_CASE("Render returns unexpected when variable is undefined", "[render]") {
    const auto* tmpl = "include/{@ project_name @}/{@project_name.hpp";

    auto vars = std::unordered_map<std::string, std::string>{};

    auto result = pgen::render(tmpl, vars);

    REQUIRE_FALSE(result);
    CHECK(result.error() == "Template variable `project_name` not defined");
}

TEST_CASE("Generate") {
    auto dest = fs::path("test_destination");

    auto tmpl = pgen::rendered_template{
        .directories = {"testdir1", "testdir2/subdir1", "testdir2/subdir2"},
        .files =
            {
                {"manifest.json", R"({"name": "main"})"},
                {"src/main.cpp", "auto main() -> int {return 0;}"},
            },
    };

    auto result = pgen::write_template(dest, tmpl);

    if(!result) {
        std::cerr << result.error() << '\n';
    }

    CHECK(result);

    CHECK(fs::exists(dest / "testdir1"));
    CHECK(fs::exists(dest / "testdir2"));
    CHECK(fs::exists(dest / "testdir2" / "subdir1"));
    CHECK(fs::exists(dest / "testdir2" / "subdir2"));
    CHECK(fs::exists(dest / "manifest.json"));
    CHECK(fs::exists(dest / "src" / "main.cpp"));

    auto file         = std::ifstream(dest / "manifest.json");
    auto manifest_str = std::stringstream();
    manifest_str << file.rdbuf();
    file.close();

    file          = std::ifstream(dest / "src" / "main.cpp");
    auto main_str = std::stringstream();
    main_str << file.rdbuf();
    file.close();

    fs::remove_all(dest);

    CHECK(manifest_str.str() == "{\"name\": \"main\"}");
    CHECK(main_str.str() == "auto main() -> int {return 0;}");
}

TEST_CASE("From directory") {
    auto dir = fs::path("test_dir");

    fs::create_directory(dir);
    fs::create_directory(dir / "subdir1");
    fs::create_directory(dir / "subdir2");

    auto f1 = std::ofstream(dir / "subdir1" / "file1.txt");
    f1 << "Hello world";
    f1.close();

    auto f2 = std::ofstream(dir / "file.json");
    f2 << R"({"name" : "dave"})";
    f2.close();

    auto result = pgen::from_directory(dir, {});

    fs::remove_all(dir);

    REQUIRE(result);
    CHECK(result->files.size() == 2);

    auto tmpl_f1 = std::ranges::find_if(result->files, [&](auto f) { return f.path == "file.json"; });

    CHECK(tmpl_f1 != result->files.end());
    CHECK(tmpl_f1->content);
    CHECK(*(tmpl_f1->content) == "{\"name\" : \"dave\"}");

    auto tmpl_f2 = std::ranges::find_if(result->files, [&](auto f) { return f.path == fs::path("subdir1/file1.txt"); });
    CHECK(tmpl_f2 != result->files.end());
    CHECK(tmpl_f2->content);
    CHECK(*(tmpl_f2->content) == "Hello world");

    CHECK(std::ranges::find(result->directories, "subdir1") != result->directories.end());
    CHECK(std::ranges::find(result->directories, "subdir2") != result->directories.end());
}

TEST_CASE("Render project template", "[render_template]") {
    auto proj_tmpl = pgen::project_template{
        .variables   = {{"project_name"}, {"namespace"}},
        .directories = {"include/{@ project_name @}", "src"},
        .files       = {{"src/{@ namespace @}/main.cpp", "hello {@ namespace @}, from {@ project_name @}"}},
    };
    auto defs = std::unordered_map<std::string, std::string>{
        {"project_name", "project"},
        {"namespace", "namespace"},
    };

    auto result = pgen::render_template(proj_tmpl, defs);

    REQUIRE(result);
    CHECK(contains(result->directories, "include/project"));
    CHECK(contains(result->directories, "src"));
    CHECK(result->files["src/namespace/main.cpp"] == "hello namespace, from project");
}