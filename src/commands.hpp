#pragma once

#include <arg_parser.hpp>
#include <toml.hpp>

#include "error/error.hpp"
#include "utility/utility.hpp"

#include <filesystem>
#include <format>
#include <iostream>

namespace tram {

inline auto HELP_ACTION = [](const psap::ArgParser& parser, [[maybe_unused]] const auto& _) {
    parser(parser[0]);
};

inline auto NEW_ACTION = [](const psap::ArgParser& parser, const psap::Command& cmd) noexcept {
    std::filesystem::path project_name { tram::string::normalize_filename(parser[0]) };

    if (project_name.empty()) {
        std::cout << "Please specify a name!" << std::endl;
        return;
    }

    if (auto error = fs::create_dir_if_notexists(project_name)) {
        if (error.is(ErrorCode::DirAlreadyExists))
            std::cout << psap::color::red << std::format("There is already a directory called '{}'.", project_name.string()) << psap::color::reset << std::endl;
        else
            error.report();

        return;
    }

    fs::create_file(project_name / fs::TRAM_PROJECT_FILE, std::format("name = \"{}\"\n", parser[0]));

    if (cmd["--sample"])
        fs::create_sample_files(project_name);

    std::cout << "Initialized project in '" << psap::color::light_magenta << std::format("./{}", project_name.string()) << psap::color::reset << "'" << std::endl;
};

inline auto ADD_ACTION = []([[maybe_unused]] const auto& _parser, [[maybe_unused]] const auto& _cmd) {
};

inline auto REMOVE_ACTION = []([[maybe_unused]] const auto& _parser, [[maybe_unused]] const auto& _cmd) {
};

inline auto BUILD_ACTION = []([[maybe_unused]] const auto& _parser, [[maybe_unused]] const auto& _cmd) {
};

inline auto RUN_ACTION = []([[maybe_unused]] const auto& _parser, [[maybe_unused]] const auto& _cmd) {
};

inline auto VERSION_ACTION = []([[maybe_unused]] const auto& _parser, [[maybe_unused]] const auto& _cmd) {
    std::cout << "tram v2024.10.1-dp" << std::endl;
};

inline auto LICENCE_ACTION = []([[maybe_unused]] const auto& _parser, [[maybe_unused]] const auto& _cmd) {
    std::cout
        << std::format("\n{}\n{}\n\n{}\n\n\n{}\n{}\n\n{}\n",
               "ArgParser by Paulanerus",
               "https://github.com/Paulanerus/ArgParser",
               psap::LICENSE_NOTICE,
               "toml11 by ToruNiina",
               "https://github.com/ToruNiina/toml11",
               toml::license_notice())
        << std::endl;
};
}
