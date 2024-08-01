#pragma once

#include <arg_parser.hpp>

inline auto HELP_ACTION = [](const psap::ArgParser &parser, [[maybe_unused]] const auto &_)
{
    parser(parser[0]);
};