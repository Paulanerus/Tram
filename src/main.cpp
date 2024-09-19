#include <arg_parser.hpp>

#include "commands.hpp"
#include "config.hpp"

int main(int argc, char* argv[])
{
    toml::color::enable();

    psap::ArgParser parser {
        psap::ParserConf { .name = "tram" }
    };

    tram::load_config();

    parser.command("help", "h")
        .help("Displays help for a tram subcommand.")
        .fallback()
        .action(HELP_ACTION);

    parser.command("new", "n", "create", "c")
        .help("Creates a new project.")
        .option(psap::Option::Flag({ "--git", "-g" }, "Initializes a git repo."))
        .option(psap::Option::Flag({ "--sample", "-s" }, "Create sample files like README.md, LICENSE.md, etc."))
        .action(NEW_ACTION);

    parser.command("add", "a")
        .help("Adds a new dependency.")
        .option(psap::Option::Value({ "--branch", "-b" }, "Select a specific branch."))
        .option(psap::Option::Flag({ "--link", "-l" }, "Links the specified library."))
        .action(ADD_ACTION);

    parser.command("remove", "rm", "delete", "del")
        .help("Removes a dependency.")
        .action(REMOVE_ACTION);

    parser.command("build", "b")
        .help("Builds the project.")
        .option(psap::Option::Flag({ "--debug", "-d" }, "Runs debug build."))
        .option(psap::Option::Flag({ "--release", "-r" }, "Runs release build."))
        .action(BUILD_ACTION);

    parser.command("run", "r")
        .help("Executes the project and perform a build, if needed.")
        .option(psap::Option::Flag({ "--debug", "-d" }, "Runs debug build."))
        .option(psap::Option::Flag({ "--release", "-r" }, "Runs release build."))
        .action(RUN_ACTION);

    parser.command("version", "v")
        .help("Displays the current version.")
        .action(VERSION_ACTION);

    parser.parse(argv, argc);
}
