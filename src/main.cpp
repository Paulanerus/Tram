#include <arg_parser.hpp>
#include <toml.hpp>

#include "commands.hpp"

int main(int argc, char *argv[])
{
    toml::color::enable();

    psap::ArgParser parser{"tram"};

    parser.command("help", "h")
        .help("Displays help for a tram subcommand")
        .action(HELP_ACTION);

    parser.parse(argv, argc);
}