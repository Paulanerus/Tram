#include <iostream>

#include "cli/arg_parser.hpp"

int main(int argc, char *argv[])
{
    auto args = Tram::Util::AsVector(argc, argv, 1);

    Tram::CLI::handleArgs(std::move(args));

    return EXIT_SUCCESS;
}