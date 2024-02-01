#pragma once

#include "../util/strings.hpp"

#include <unordered_map>
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
            const std::string description;

            const std::vector<std::string> alternatives;

            const std::vector<std::string> allowed_args;

            const std::function<void(const std::vector<std::string> &&args)> function;
        };

        class ArgParser
        {
        public:
            ArgParser()
            {
                auto help_cmd = Command{.description = "Shows help",
                                        .alternatives = {"h"},
                                        .allowed_args = {},
                                        .function = [this](const std::vector<std::string> &args)
                                        {
                                            std::cout << "\n--------<Tram v2024.1.1>--------\n\n";

                                            for (auto &[id, cmd] : m_Commands)
                                                std::cout << "\t" << id << " - " << cmd.description << "\n";

                                            std::cout << std::endl;
                                        }};

                m_Commands.insert(std::make_pair("help", help_cmd));

                auto new_cmd = Command{.description = "Creates a new  project",
                                       .alternatives = {"create", "c", "n"},
                                       .allowed_args = {""},
                                       .function = [](const std::vector<std::string> &args)
                                       {
                                           std::cout << "Creating new project..." << std::endl;
                                       }};

                m_Commands.insert(std::make_pair("new", new_cmd));

                auto add_cmd = Command{.description = "Adds a new dependency",
                                       .alternatives = {"a"},
                                       .allowed_args = {"--branch"},
                                       .function = [](const std::vector<std::string> &args)
                                       {
                                           std::cout << "Adding dependency..." << std::endl;
                                       }};

                m_Commands.insert(std::make_pair("add", add_cmd));

                auto remove_cmd = Command{.description = "Removes a dependency",
                                          .alternatives = {"rm", "delete", "del"},
                                          .allowed_args = {},
                                          .function = [](const std::vector<std::string> &args)
                                          {
                                              std::cout << "Removing dependency..." << std::endl;
                                          }};

                m_Commands.insert(std::make_pair("remove", remove_cmd));

                auto build_cmd = Command{.description = "Builds the project",
                                         .alternatives = {"b"},
                                         .allowed_args = {"debug", "release"},
                                         .function = [](const std::vector<std::string> &args)
                                         {
                                             std::cout << "Building project..." << std::endl;
                                         }};

                m_Commands.insert(std::make_pair("build", build_cmd));

                auto run_cmd = Command{.description = "Runs the project",
                                       .alternatives = {"b"},
                                       .allowed_args = {"debug", "release"},
                                       .function = [](const std::vector<std::string> &args)
                                       {
                                           std::cout << "Running project..." << std::endl;
                                       }};

                m_Commands.insert(std::make_pair("run", run_cmd));
            }

            void handle(std::vector<std::string> &&args)
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
            std::unordered_map<std::string, Command> m_Commands;

            std::optional<Command> getCommandByIdentifier(const std::string &identifier)
            {
                if (m_Commands.find(identifier) != m_Commands.end())
                    return m_Commands[identifier];

                for (auto &[id, cmd] : m_Commands)
                {
                    for (auto &alt : cmd.alternatives)
                    {
                        if (alt == identifier)
                            return cmd;
                    }
                }

                return {};
            }
        };
    }
}