#pragma once

#include <arg_parser.hpp>

#include "project_wizard.hpp"

inline auto HELP_ACTION = [](const psap::ArgParser &parser, [[maybe_unused]] const auto &_)
{
    parser(parser[0]);
};

inline auto NEW_ACTION = [](const psap::ArgParser &parser, const psap::Command &cmd) noexcept
{
    auto project_name = parser[0];

    if (project_name.empty())
    {
        std::cout << "Please specify a name!" << std::endl;
        return;
    }

    tram::create_project(project_name, cmd["--git"]);
};

inline auto ADD_ACTION = []([[maybe_unused]] const auto &_parser, [[maybe_unused]] const auto &_cmd) {
};

inline auto REMOVE_ACTION = []([[maybe_unused]] const auto &_parser, [[maybe_unused]] const auto &_cmd) {
};

inline auto BUILD_ACTION = []([[maybe_unused]] const auto &_parser, [[maybe_unused]] const auto &_cmd) {
};

inline auto RUN_ACTION = []([[maybe_unused]] const auto &_parser, [[maybe_unused]] const auto &_cmd) {
};

inline auto VERSION_ACTION = []([[maybe_unused]] const auto &_parser, [[maybe_unused]] const auto &_cmd)
{
    std::cout << "tram v2024.8.1-dp" << std::endl;
};