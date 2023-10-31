#include <iostream>
#include <string>
#include <vector>

#include <argparse/argparse.hpp>

#include "cmd.hpp"

namespace ap = argparse;

auto main(int argc, char* argv[]) -> int {
    auto cli = ap::ArgumentParser{"pgen", "0.2.0"};

    auto gen = ap::ArgumentParser{"gen"};
    gen.add_description("Generate project from a template file");
    gen.add_argument("destination").help("Directory where project will be generated.");
    gen.add_argument("template").help("Path to template.");

    auto fd = ap::ArgumentParser{"fd"};
    fd.add_description("Generate template file from a directory.");
    fd.add_argument("dir").help("Directory to generate template from.");
    fd.add_argument("-o", "--output").help("File to output template to.");

    cli.add_subparser(fd);
    cli.add_subparser(gen);

    try {
        cli.parse_args(argc, argv);
    } catch(const std::exception& err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }

    if(cli.is_subcommand_used(gen)) {
        auto dest = gen.get<std::string>("destination");
        auto temp = gen.get<std::string>("template");

        auto cmd = pgen::generate{dest, temp};
        return cmd.execute();
    }

    if(cli.is_subcommand_used(fd)) {
        auto dir = fd.get<std::string>("dir");
        auto out = fd.present("-o");

        auto cmd = pgen::from_directory{dir, out};
        return cmd.execute();
    }

    std::cout << cli;
    return 0;
}