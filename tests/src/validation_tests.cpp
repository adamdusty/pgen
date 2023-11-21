#include "validation.hpp"
#include <nlohmann/json.hpp>
#include <snitch/snitch.hpp>

TEST_CASE("No vars array return valid", "[validation]") {
    auto js = R"(
        {}
    )"_json;

    auto result = pgen::validate_json(js);

    CHECK(result.valid);
}

TEST_CASE("Vars is not an array") {
    auto v1 = R"(
        {"vars": "hello"}
    )"_json;

    auto v2 = R"(
        {"vars": {"hello": 1}}
    )"_json;

    auto result = pgen::validate_json(v1);
    CHECK_FALSE(result.valid);
    CHECK(result.message == "`vars` is not an array.");

    result = pgen::validate_json(v1);
    CHECK_FALSE(result.valid);
    CHECK(result.message == "`vars` is not an array.");
}

TEST_CASE("Invlaid vars: var is not string", "[validation]") {
    auto js = R"(
        {
            "vars": [
                "hello",
                1,
                2,
                "goodbye"
            ]
        }
    )"_json;

    auto result = pgen::validate_json(js);

    CHECK_FALSE(result.valid);
    CHECK(result.message == "var `1` is not a string.");
}

TEST_CASE("Valid vars array returns valid result", "[validation]") {
    auto js = R"(
        {
            "vars": [
                "hello",
                "goodbye"
            ]
        }
    )"_json;

    auto result = pgen::validate_json(js);

    CHECK(result.valid);
}

TEST_CASE("No points returns valid result", "[validation]") {
    auto js = R"(
        {}
    )"_json;

    auto result = pgen::validate_json(js);

    CHECK(result.valid);
}

TEST_CASE("Invalid when points is not an array", "[validation]") {
    auto js = R"(
        {
            "points": {
                "hi": 1
            }
        }
    )"_json;

    auto result = pgen::validate_json(js);

    CHECK_FALSE(result.valid);
    CHECK(result.message == "`points` is not an array.");
}

TEST_CASE("Invalid when point is not an object", "[validation]") {
    auto js = R"(
        {
            "points": [
                {
                    "content": "hi",
                    "dir": false,
                    "path": "./here"
                },
                "hello"
            ]
        }
    )"_json;

    auto result = pgen::validate_json(js);

    CHECK_FALSE(result.valid);
    CHECK(result.message == "point is not an object");
}

TEST_CASE("Invalid when points element doesn't contain all fields", "[validation]") {
    auto js = R"(
        {
            "points": [
                {
                    "content": "hi",
                    "dir": false,
                    "path": "./here"
                },
                {
                    "dir": false
                }
            ]
        }
    )"_json;

    auto result = pgen::validate_json(js);

    CHECK_FALSE(result.valid);
    CHECK(result.message == "point `{\"dir\":false}` is doesn't define all fields.");
}

TEST_CASE("Valid points", "[validation]") {
    auto js = R"(
        {
            "points": [
                {
                    "content": "hi",
                    "dir": false,
                    "path": "./here"
                },
                {
                    "content": "hello",
                    "dir": false,
                    "path": "./there"
                }
            ]
        }
    )"_json;

    auto result = pgen::validate_json(js);

    CHECK(result.valid);
}

TEST_CASE("Valid template", "[validation]") {
    auto js =
        R"--({"points":[{"content":"---\nAccessModifierOffset: -4\nAlignConsecutiveAssignments: true\nAlignTrailingComments: true\nAllowAllConstructorInitializersOnNextLine: true\nAllowShortBlocksOnASingleLine: true\nAllowShortCaseLabelsOnASingleLine: false\nAllowShortFunctionsOnASingleLine: Inline\nAllowShortIfStatementsOnASingleLine: false\nAllowShortLambdasOnASingleLine: Inline\nAllowShortLoopsOnASingleLine: false\nAlwaysBreakBeforeMultilineStrings: false\nAlwaysBreakTemplateDeclarations: Yes\nBinPackArguments: false\nBinPackParameters: false\nBreakBeforeBraces: Attach\nBreakBeforeTernaryOperators: true\nBreakConstructorInitializers: AfterColon\nColumnLimit: 120\nCompactNamespaces: false\nIndentWidth: 4\nPointerAlignment: Left\nSortIncludes: true\nSortUsingDeclarations: true\nSpaceAfterCStyleCast: false\nSpaceAfterLogicalNot: false\nSpaceAfterTemplateKeyword: false\nSpaceBeforeCpp11BracedList: false\nSpaceBeforeInheritanceColon: true\nSpaceBeforeParens: Never\nSpaceBeforeRangeBasedForLoopColon: false\nSpaceInEmptyParentheses: false\nSpacesInAngles: false\nSpacesInCStyleCastParentheses: false\nSpacesInContainerLiterals: false\nSpacesInParentheses: false\nSpacesInSquareBrackets: false\n","dir":false,"path":".clang-format"},{"content":"Checks: \"*,\n  -abseil-*,\n  -altera-*,\n  -android-*,\n  -fuchsia-*,\n  -google-*,\n  -llvm*,\n  -zircon-*,\n  -readability-else-after-return,\n  -readability-static-accessed-through-instance,\n  -readability-avoid-const-params-in-decls,\n  -readability-magic-numbers,\n  -cppcoreguidelines-non-private-member-variables-in-classes,\n  -cppcoreguidelines-avoid-magic-numbers,\n  -misc-non-private-member-variables-in-classes,\n  -modernize-use-nodiscard,\n\"\n\nCheckOptions:\n  - key: readability-identifier-length.IgnoredVariableNames\n    value: 'a|b|x|y|z'\n  - key: readability-identifier-length.IgnoredParameterNames\n    value: 'a|b|x|y|z'\n  - key: readability-identifier-length.MinimumVariableNameLength\n    value: 2","dir":false,"path":".clang-tidy"},{"content":".idea/\n.vs/\n.vscode/\nbuild/\ncmake/open-cpp-coverage.cmake\ncmake-build-*/\nprefix/\nCMakeLists.txt.user\nCMakeUserPresets.json\n","dir":false,"path":".gitignore"},{"content":"","dir":true,"path":"cmake"},{"content":"function(add_dev_compile_opts target)\n    target_compile_options(${target} PRIVATE\n        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:\n            -U_FORTIFY_SOURCE\n            -D_FORTIFY_SOURCE=3\n            -fstack-protector-strong\n            -fcf-protection=full\n            -fstack-clash-protection\n            -Wall\n            -Wextra\n            -Wpedantic\n            -Wconversion\n            -Wsign-conversion\n            -Wcast-qual\n            -Wformat=2\n            -Wundef\n            -Werror=float-equal\n            -Wshadow\n            -Wcast-align\n            -Wunused\n            -Wnull-dereference\n            -Wdouble-promotion\n            -Wimplicit-fallthrough\n            -Wextra-semi\n            -Woverloaded-virtual\n            -Wnon-virtual-dtor\n            -Wold-style-cast>\n            \n        $<$<CXX_COMPILER_ID:MSVC>:\n            /sdl\n            /guard:cf\n            /utf-8\n            /diagnostics:caret\n            /w14165\n            /w44242\n            /w44254\n            /w44263\n            /w34265\n            /w34287\n            /w44296\n            /w44365\n            /w44388\n            /w44464\n            /w14545\n            /w14546\n            /w14547\n            /w14549\n            /w14555\n            /w34619\n            /w34640\n            /w24826\n            /w14905\n            /w14906\n            /w14928\n            /w45038\n            /W4\n            /permissive-\n            /volatile:iso\n            /Zc:inline\n            /Zc:preprocessor\n            /Zc:enumTypes\n            /Zc:lambda\n            /Zc:__cplusplus\n            /Zc:externConstexpr\n            /Zc:throwingNew\n            /EHsc>\n    )\nendfunction()","dir":false,"path":"cmake\\compile-options.cmake"},{"content":"","dir":false,"path":"cmake\\dependencies.cmake"},{"content":"include(\"${CMAKE_CURRENT_LIST_DIR}/{@ project_name @}Targets.cmake\")","dir":false,"path":"cmake\\install-config.cmake"},{"content":"if(PROJECT_IS_TOP_LEVEL)\n  set(\n      CMAKE_INSTALL_INCLUDEDIR \"include/{@ project_name @}-${PROJECT_VERSION}\"\n      CACHE PATH \"\"\n  )\nendif()\n\nset(package {@ project_name @})\ninclude(GNUInstallDirs)\ninclude(CMakePackageConfigHelpers)\n\n# Set up export components\ninstall(\n    TARGETS {@ namespace @}_{@ project_name @}\n    EXPORT {@ project_name @}Targets\n    RUNTIME COMPONENT {@ project_name @}_Runtime\n    LIBRARY COMPONENT {@ project_name @}_Runtime\n    NAMELINK_COMPONENT {@ project_name @}_Development\n    ARCHIVE COMPONENT {@ project_name @}_Development\n    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}\n)\n\n# Write package file for installations\nwrite_basic_package_version_file(\n    \"${package}ConfigVersion.cmake\"\n    VERSION ${PACKAGE_VERSION}\n    COMPATIBILITY SameMajorVersion\n)\n\n# Allow package maintainers to freely override the path for the configs\nset(\n    {@ project_name @}_INSTALL_CMAKEDIR \"${CMAKE_INSTALL_LIBDIR}/cmake/${package}\"\n    CACHE PATH \"CMake package config location relative to the install prefix\"\n)\nmark_as_advanced({@ project_name @}_INSTALL_CMAKEDIR)\n\n# Install config file to the install directory\n# Config file is used by find_package() (I assume)\ninstall(\n    FILES cmake/install-config.cmake\n    DESTINATION \"${{@ project_name @}_INSTALL_CMAKEDIR}\"\n    RENAME \"${package}Config.cmake\"\n    COMPONENT {@ project_name @}_Development\n)\n\n# Install ConfigVersion file\n# More package install info\ninstall(\n    FILES \"${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake\"\n    DESTINATION \"${{@ project_name @}_INSTALL_CMAKEDIR}\"\n    COMPONENT {@ project_name @}_Development\n)\n\n# Install the dev component from targets defined above (I think)\ninstall(\n    EXPORT {@ project_name @}Targets\n    NAMESPACE {@ namespace @}::\n    DESTINATION \"${{@ project_name @}_INSTALL_CMAKEDIR}\"\n    COMPONENT {@ project_name @}_Development\n)\n\n# Include CPack\nif(PROJECT_IS_TOP_LEVEL)\n  include(CPack)\nendif()","dir":false,"path":"cmake\\install-rules.cmake"},{"content":"option({@ upper(project_name) @}_BUILD_TESTS \"Build tests for {@ project_name @}\" OFF)","dir":false,"path":"cmake\\options.cmake"},{"content":"# ---- Warning guard ----\n\n# target_include_directories with the SYSTEM modifier will request the compiler\n# to omit warnings from the provided paths, if the compiler supports that\n# This is to provide a user experience similar to find_package when\n# add_subdirectory or FetchContent is used to consume this project\nset(warning_guard \"\")\nif(NOT PROJECT_IS_TOP_LEVEL)\n    set(warning_guard SYSTEM)\nendif()\n","dir":false,"path":"cmake\\warning-guard.cmake"},{"content":"cmake_minimum_required(VERSION 3.27)\n\nproject(\n    {@ project_name @}\n    VERSION 0.1.0\n    DESCRIPTION \"A short description\"\n    LANGUAGES CXX\n)\n\n# Include utilities\ninclude(cmake/options.cmake)\ninclude(cmake/warning-guard.cmake)\ninclude(cmake/compile-options.cmake)\ninclude(cmake/dependencies.cmake)\n\n# Declare library\nadd_library({@ namespace @}_{@ project_name @})\nadd_library({@ namespace @}::{@ project_name @} ALIAS {@ namespace @}_{@ project_name @})\n\n# Add sources\ntarget_sources({@ namespace @}_{@ project_name @} PRIVATE\n    src/{@ project_name @}.cpp\n)\n\n# Add include directory\n# warning_guard adds SYSTEM modifier if the project isn't top level\ntarget_include_directories(\n    {@ namespace @}_{@ project_name @} ${warning_guard}\n    PUBLIC\n    \"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>\"\n)\n\n# Include and link dependencies\n#target_link_libraries({@ namespace @}_{@ project_name @})\n\n# Add compile options\nadd_dev_compile_opts({@ namespace @}_{@ project_name @})\n\n# Generate header for export\ninclude(GenerateExportHeader)\ngenerate_export_header(\n    {@ namespace @}_{@ project_name @}\n    BASE_NAME {@ project_name @}\n    EXPORT_FILE_NAME include/{@ namespace @}/export.hpp\n    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251\n)\n\n# Set properties\nset_target_properties(\n    {@ namespace @}_{@ project_name @} PROPERTIES\n    CXX_VISIBILITY_PRESET \"hidden\"\n    VISIBILITY_INLINES_HIDDEN YES\n    VERSION \"${PROJECT_VERSION}\"\n    SOVERSION \"${PROJECT_VERSION_MAJOR}\"\n    EXPORT_NAME {@ project_name @}\n    OUTPUT_NAME {@ project_name @}\n)\n\n# Install rules\nif(NOT CMAKE_SKIP_INSTALL_RULES)\n  include(cmake/install-rules.cmake)\nendif()\n\n# Enable testing\nif({@ upper(project_name) @}_BUILD_TESTS)\n    include(CTest)\n    add_subdirectory(tests)\nendif()","dir":false,"path":"CMakeLists.txt"},{"content":"{\n    \"version\": 6,\n    \"cmakeMinimumRequired\": {\n        \"major\": 3,\n        \"minor\": 27,\n        \"patch\": 0\n    },\n    \"configurePresets\": [\n        {\n            \"name\": \"base\",\n            \"hidden\": true,\n            \"cacheVariables\": {\n                \"CMAKE_CXX_EXTENSIONS\": \"OFF\",\n                \"CMAKE_CXX_STANDARD\": \"20\",\n                \"CMAKE_COLOR_DIAGNOSTICS\": \"ON\"\n            }\n        },\n        {\n            \"name\": \"debug\",\n            \"binaryDir\": \"./build/debug\",\n            \"inherits\": [\n                \"base\"\n            ],\n            \"cacheVariables\": {\n                \"CMAKE_BUILD_TYPE\": \"debug\"\n            }\n        },\n        {\n            \"name\": \"release\",\n            \"binaryDir\": \"./build/release\",\n            \"inherits\": [\n                \"base\"\n            ],\n            \"cacheVariables\": {\n                \"CMAKE_BUILD_TYPE\": \"release\"\n            }\n        },\n        {\n            \"name\": \"dev\",\n            \"inherits\": [\n                \"debug\"\n            ],\n            \"cacheVariables\": {\n                \"CMAKE_EXPORT_COMPILE_COMMANDS\": \"ON\",\n                \"{@ upper(project_name) @}_BUILD_TESTS\": \"ON\"\n            }\n        }\n    ],\n    \"buildPresets\": [\n        {\n            \"name\": \"dev\",\n            \"configurePreset\": \"dev\"\n        },\n        {\n            \"name\": \"debug\",\n            \"configurePreset\": \"debug\"\n        },\n        {\n            \"name\": \"release\",\n            \"configurePreset\": \"release\"\n        }\n    ],\n    \"testPresets\": [\n        {\n            \"name\": \"dev\",\n            \"configurePreset\": \"dev\",\n            \"configuration\": \"Debug\",\n            \"output\": {\n                \"outputOnFailure\": true\n            }\n        }\n    ]\n}","dir":false,"path":"CMakePresets.json"},{"content":"","dir":true,"path":"include"},{"content":"","dir":true,"path":"include\\{@ lower(namespace) @}"},{"content":"#pragma once\n\n#include <cstdint>\n\nnamespace {@ lower(namespace) @} {\nauto get_version() -> std::uint64_t;\n}","dir":false,"path":"include\\{@ lower(namespace) @}\\{@ lower(project_name) @}.hpp"},{"content":"","dir":true,"path":"src"},{"content":"#include \"{@ lower(namespace) @}/{@ lower(project_name) @}.hpp\"\n\nnamespace {@ lower(namespace) @} {\nauto get_version() -> std::uint64_t{ return 1; }\n}","dir":false,"path":"src\\{@ lower(project_name) @}.cpp"},{"content":"","dir":true,"path":"tests"},{"content":"cmake_minimum_required(VERSION 3.27)\n\nproject(\n    {@ project_name @}Tests\n    LANGUAGES CXX\n)\n\n# If test project is built independently, find the project package\nif(PROJECT_IS_TOP_LEVEL)\n    find_package({@ project_name @} REQUIRED)\n    enable_testing()\nendif()\n\n# Get the testing library\ninclude(FetchContent)\nFetchContent_Declare(\n    snitch\n    GIT_REPOSITORY https://github.com/cschreib/snitch.git\n    GIT_TAG        v1.2.2\n    GIT_SHALLOW    TRUE\n    SYSTEM\n    FIND_PACKAGE_ARGS # Use system package if installed\n)\nFetchContent_MakeAvailable(snitch)\n\n# Add test exe\nadd_executable({@ project_name @}_tests)\n\n# Add test sources\ntarget_sources({@ project_name @}_tests PRIVATE\n    src/{@ project_name @}_test.cpp\n)\n\n# Link libs\ntarget_link_libraries({@ project_name @}_tests PRIVATE {@ namespace @}::{@ project_name @} snitch::snitch)\n\n# Add test\nadd_test(NAME {@ project_name @}_tests COMMAND {@ project_name @}_tests)\n\n# Enable test binary to find the DLL if building a shared lib\nif(BUILD_SHARED_LIBS)\n    set_tests_properties(xyz_tests PROPERTIES\n        ENVIRONMENT_MODIFICATION\n            \"PATH=path_list_append:$<TARGET_FILE_DIR:xyz::xyz>;PATH=path_list_append:$<TARGET_FILE_DIR:snitch::snitch>\"\n    )\nendif()\n","dir":false,"path":"tests\\CMakeLists.txt"},{"content":"","dir":true,"path":"tests\\src"},{"content":"#include <snitch/snitch.hpp>\n#include <{@ lower(namespace) @}/{@ lower(project_name) @}.hpp>\n\nTEST_CASE(\"Test case\", \"[greeting]\") {\n    REQUIRE({@ lower(namespace) @}::get_version() == 1);\n}","dir":false,"path":"tests\\src\\{@ lower(project_name) @}_test.cpp"}],"vars":["namespace","project_name"]})--"_json;

    auto result = pgen::validate_json(js);

    CHECK(result.valid);
}