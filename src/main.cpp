#include "cli/arg_parser.hpp"
#include "config/config.hpp"

#include <toml.hpp>

int main(int argc, char *argv[])
{
    toml::color::enable();

    std::vector<std::string> args{argv + 1, argv + argc};
    std::for_each(args.begin(), args.end(), [](std::string &str_in)
                  { Tram::Util::ToLower(str_in); });

    Tram::CLI::ArgParser parser;
    parser.handle(std::move(args));

    return EXIT_SUCCESS;
}