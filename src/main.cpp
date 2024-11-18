#include <arg_parser.hpp>
#include <toml.hpp>

#include "commands.hpp"
#include "curl/curl_client.hpp"

int main(int argc, char* argv[])
{
    toml::color::enable();
    tram::curl::global_init();

    psap::ArgParser parser {
        psap::ParserConf { .name = "tram", .unknown_option_policy = psap::UnknownOptionPolicy::ReportRemove }
    };

    parser.option(psap::make_flag("--verbose", "-v", "Print various debugging information."));

    parser.command("help", "h")
        .help("Displays help for a tram subcommand.")
        .fallback()
        .action(tram::HELP_ACTION);

    parser.command("new", "n", "create", "c")
        .help("Creates a new project.")
        .option(psap::make_flag("--git", "-g", "Initializes a git repo."))
        .option(psap::make_flag("--sample", "-s", "Create sample files like README.md, LICENSE.md, etc."))
        .action(tram::NEW_ACTION);

    parser.command("libs", "l")
        .help("Lists every library and its status.")
        .option(psap::make_flag("--installed", "-i", "View filter that lists only installed libraries."))
        .action(tram::LIBS_ACTION);

    parser.command("add", "a")
        .help("Adds a new library.")
        .option(psap::make_value("--branch", "-b", "Select a specific branch."))
        .action(tram::ADD_ACTION);

    parser.command("remove", "rem", "rm", "delete", "del")
        .help("Removes a library.")
        .action(tram::REMOVE_ACTION);

    parser.command("build", "b")
        .help("Builds the project.")
        .option(psap::make_value("--config", "-c", "Selects a specific config."))
        .action(tram::BUILD_ACTION);

    parser.command("run", "r")
        .help("Executes the project and perform a build, if needed.")
        .option(psap::make_value("--config", "-c", "Selects a specific config."))
        .action(tram::RUN_ACTION);

    parser.command("version", "v")
        .help("Displays the current version.")
        .action(tram::VERSION_ACTION);

    parser.command("license")
        .help("Print dependency licenses.")
        .action(tram::LICENCE_ACTION);

    parser.parse(argv, argc);
}
