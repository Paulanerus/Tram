#include "cli/arg_parser.hpp"

int main(int argc, char *argv[])
{
    auto args = Tram::Util::AsVector(argc, argv, 1);

    Tram::CLI::ArgParser parser;
    parser.handle(std::move(args));

    return EXIT_SUCCESS;
}