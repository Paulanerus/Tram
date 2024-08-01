#pragma once

#include <arg_parser.hpp>

inline auto HELP_ACTION = [](const psap::ArgParser &parser, [[maybe_unused]] const auto &_)
{
    parser(parser[0]);
};

inline auto NEW_ACTION = []([[maybe_unused]] const auto &_parser, [[maybe_unused]] const auto &_cmd) {
};

inline auto ADD_ACTION = []([[maybe_unused]] const auto &_parser, [[maybe_unused]] const auto &_cmd) {
};

inline auto REMOVE_ACTION = []([[maybe_unused]] const auto &_parser, [[maybe_unused]] const auto &_cmd) {
};

inline auto BUILD_ACTION = []([[maybe_unused]] const auto &_parser, [[maybe_unused]] const auto &_cmd) {
};

inline auto RUN_ACTION = []([[maybe_unused]] const auto &_parser, [[maybe_unused]] const auto &_cmd) {
};