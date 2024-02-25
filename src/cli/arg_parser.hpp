#pragma once

#include "../util/strings.hpp"
#include "../util/system.hpp"
#include "../config/config.hpp"
#include "../util/tram.hpp"

#include <algorithm>
#include <functional>
#include <optional>
#include <iostream>
#include <vector>

namespace Tram
{
    namespace CLI
    {
        struct Command
        {
            const std::string identifier;

            const std::string description;

            const StringVec alternatives;

            const StringVec allowed_args;

            const std::function<void(StringVec &&args)> function;

            Command(std::string &&identifier, std::string &&description, StringVec &&alternatives, StringVec &&allowed_args, std::function<void(StringVec &&args)> &&function)
                : identifier(std::move(identifier)),
                  description(std::move(description)),
                  alternatives(std::move(alternatives)),
                  allowed_args(std::move(allowed_args)),
                  function(std::move(function)) {}
        };

        class ArgParser
        {
        public:
            ArgParser()
            {
                m_Commands.emplace_back(
                    "help",
                    "Shows help",
                    StringVec{"h"},
                    StringVec{},
                    [this](StringVec &&args)
                    {
                        std::cout << "\n--------<" << Tram::TRAM_NAME << " " << Tram::TRAM_VERSION << ">--------\n\n";

                        for (auto &cmd : m_Commands)
                            std::cout << "\t" << cmd.identifier << " - " << cmd.description << "\n";

                        std::cout << std::endl;
                    });

                m_Commands.emplace_back(
                    "new",
                    "Creates a new project",
                    StringVec{"create", "c", "n"},
                    StringVec{},
                    [this](StringVec &&args)
                    {
                        projectWizard(std::move(args));
                    });

                m_Commands.emplace_back(
                    "add",
                    "Adds a new dependency",
                    StringVec{"a"},
                    StringVec{"--branch", "--link"},
                    [](StringVec &&args)
                    {
                        // git add submodule libs/$dependency_name

                        std::cout << "Adding dependency..." << std::endl;
                    });

                m_Commands.emplace_back(
                    "remove",
                    "Removes a dependency",
                    StringVec{"rm", "delete", "del"},
                    StringVec{},
                    [](StringVec &&args)
                    {
                        // git rm <dependency>

                        std::cout << "Removing dependency..." << std::endl;
                    });

                m_Commands.emplace_back(
                    "build",
                    "Builds the project",
                    StringVec{"b"},
                    StringVec{"debug", "release"},
                    [](StringVec &&args)
                    {
                        // premake5 (gmake2 vsXXXX xcode)
                        //  (make | msbuild | X)

                        std::cout << "Building project..." << std::endl;
                    });

                m_Commands.emplace_back(
                    "run",
                    "Runs the project",
                    StringVec{"r"},
                    StringVec{"debug", "release", "--build"},
                    [](StringVec &&args)
                    {
                        //--build builds before runs the app

                        //./bin/(Debug | Release)/$project_name ...args
                    });
            }

            void handle(StringVec &&args)
            {
                if (args.empty())
                {
                    std::cout << "No command provided. Please type 'tram help' to list available commands." << std::endl;
                    return;
                }

                auto command = getCommandByIdentifier(args[0]);

                if (!command.has_value())
                {
                    std::cout << "Unknown command '" << args[0] << "'. Please type 'tram help' to list available commands." << std::endl;
                    return;
                }

                args.erase(args.begin());

                command.value().function(std::move(args));
            }

        private:
            std::vector<Command> m_Commands;

            std::optional<Command> getCommandByIdentifier(const std::string &identifier)
            {
                auto direct = std::find_if(m_Commands.begin(), m_Commands.end(), [&identifier](const Command &cmd)
                                           { return cmd.identifier == identifier || std::find(cmd.alternatives.begin(), cmd.alternatives.end(), identifier) != cmd.alternatives.end(); });

                return direct == m_Commands.end() ? std::nullopt : std::optional<Command>{*direct};
            }

            void projectWizard(StringVec &&args)
            {
                std::cout << "Welcome to Tram project wizard!\n\n"
                          << std::endl;

                // Project name

                // Project kind

                // Git init if not exists

                // Sample files (.gitignore, License.md, README.md, main.cpp)

                // Hint to run the project

                // Creates config based on user input

                const auto &config = Tram::TramConfig::create("MyProject", Tram::ProjectKind::APP, Tram::CppVersion::CPP_17);

                config.save();
            }
        };
    }
}