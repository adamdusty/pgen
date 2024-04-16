#include <snitch/snitch.hpp>
#include <template.hpp>

TEST_CASE("Valid template returns true", "[validate_template]") {
    const auto* tmpl = R"(
        variables:
        - identifier: project
          display_name: Project Name
          description: Name of project
        - identifier: destination
        directories:
        - testdir1
        - testdir2/{@ project @}
        files:
        - path: src/main.cpp
          content: auto main() -> int { return 0; }
    )";

    auto result = pgen::validate_template(tmpl);

    CHECK(result);
}

TEST_CASE("Invalid template returns false", "[validate_template]") {
    const auto* tmpl = R"(
        variables:
        - identifier: project
          display_name: Project Name
          description: Name of project
        - identifier: destination
        directories:
        - testdir1
        - testdir2/{@ project @}
        files:
        - path: src/main.cpp
          content: auto main() -> int { return 0; }
    )";

    auto result = pgen::validate_template(tmpl);

    CHECK_FALSE(result);
}