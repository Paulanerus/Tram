#include "cli/arg_parser.hpp"
#include "config/config.hpp"

#include <toml.hpp>

int main(int argc, char *argv[])
{
    toml::color::enable();

    std::vector<std::string> args{argv + 1, argv + argc};
    std::for_each(args.begin(), args.end(), [](std::string &str_in)
                  { Tram::Util::ToLower(str_in); });

    const auto &config = Tram::TramConfig::load();

    if (!config.has_value())
        return EXIT_FAILURE;

    Tram::CLI::ArgParser parser;
    parser.handle(std::move(args), std::move(const_cast<Tram::TramConfig &>(config.value())));

    return EXIT_SUCCESS;
}