#pragma once

#include <arg_parser.hpp>
#include <iomanip>
#include <toml.hpp>

#include "config/config.hpp"
#include "curl/curl_client.hpp"
#include "error/error.hpp"
#include "gen/make/gen_makefile.hpp"
#include "lib/lib_manager.hpp"
#include "utility/utility.hpp"

#include <filesystem>
#include <format>
#include <iostream>

#include <curl/curl.h>
#include <type_traits>

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

    fs::create_file(project_name / fs::TRAM_PROJECT_FILE, std::format("name = \"{}\"\n{}", parser[0], cmd["--sample"] ? "\n[build]\nsrc_files = [\"src/\"]\n" : ""));

    if (cmd["--sample"])
        fs::create_sample_files(project_name);

    std::cout << "Initialized project in '" << psap::color::magenta << std::format("./{}", project_name.string()) << psap::color::reset << "'" << std::endl;
};

inline auto LIBS_ACTION = []([[maybe_unused]] const auto& _parser, const psap::Command& cmd) {
    tram::load_config();

    bool has_installed_flag = cmd["--installed"];

    const auto& libs = tram::config().libraries();

    std::cout << std::endl;

    std::cout << "Libraries:" << "\n";

    for (auto& lib : libs) {

        lib::InstallLocation loc = lib::validate_install(lib);

        bool installed = loc != lib::InstallLocation::None;

        if (has_installed_flag && !installed)
            continue;

        std::cout << std::format("\t{} - [{}]\n", lib.name, (!has_installed_flag && installed ? std::format("{} {{}}", psap::color::light_green("installed"), (static_cast<std::underlying_type_t<lib::InstallLocation>>(loc) >= 2) ? "global" : "local") : psap::color::light_red("not installed")));
    }

    std::cout << std::endl;
};

inline auto ADD_ACTION = [](const psap::ArgParser& parser, const psap::Command& cmd) {
    const std::string url = parser[0];

    if (url.empty()) {
        std::cout << psap::color::light_red << "Please provide a URL to add a library." << psap::color::reset << std::endl;
        return;
    }

    tram::load_config();

    auto last_slash = url.find_last_of("/");
    internal::library lib_to_add {
        .name = cmd.get<std::string>("--alias").value_or(url.substr(last_slash == std::string::npos ? 0 : last_slash + 1)),
        .url = url,
        .branch = cmd.get<std::string>("--branch").value_or(""),
        .include_files = "",
        .src_files = "",
        .kind = "",
    };

    for (auto& lib : tram::config().libraries()) {

        if (lib == lib_to_add) {
            std::cout << psap::color::light_yellow << "The library " << std::quoted(lib_to_add.name) << " was alread added." << psap::color::reset << std::endl;
            return;
        }
    }

    tram::config().add_library(std::move(lib_to_add), cmd["--global"]);
};

inline auto REMOVE_ACTION = [](const psap::ArgParser& parser, const auto& cmd) {
    const std::string lib_name = parser[0];

    if (lib_name.empty()) {
        std::cout << "Please provide a library name." << std::endl;
        return;
    }

    tram::load_config();

    for (auto& lib : tram::config().libraries()) {

        if (lib.name != lib_name)
            continue;

        auto err = lib::remove_lib(lib, cmd["--global"]);

        if (parser["--verbose"] && err.is(ErrorCode::LibraryIsNotInstalled))
            err.report();

        if (err)
            std::cout << psap::color::light_red << "Could not remove the library " << std::quoted(lib_name) << ", cause it is not installed." << psap::color::reset << std::endl;
        else
            std::cout << "Removed " << std::quoted(psap::color::light_cyan(lib.name)) << std::endl;

        return;
    }

    std::cout << psap::color::light_red << "Missing library (" << lib_name << ") in the tram.toml." << psap::color::reset << std::endl;
};

inline auto BUILD_ACTION = []([[maybe_unused]] const auto& _parser, const psap::Command& cmd) {
    tram::load_config();

    if (fs::compare_modified_time_from_file(fs::last_modified_time(fs::TRAM_PROJECT_FILE))) {

        if (auto err = gen::create_make_file()) {
            err.report();
            return;
        }
    }

    std::filesystem::path tram_dir { fs::TRAM_DIR };

    if (std::filesystem::exists(tram_dir / "Makefile"))
        system::call(std::format("make -f {}/Makefile config={}", fs::TRAM_DIR, tram::config().build().resolve_build_config(cmd.get<std::string>("--config"))));
    else
        std::cout << "No Makfile was found in " << (tram_dir / "Makefile") << std::endl;
};

inline auto RUN_ACTION = [](const psap::ArgParser& parser, const psap::Command& cmd) {
    tram::load_config();

    const auto& build_conf = tram::config().build();

    std::string build_config = build_conf.resolve_build_config(cmd.get<std::string>("--config"));

    std::string build_dir = build_conf.out;
    string::replace_all(build_dir, "$(config)", build_config);

    std::filesystem::path path { build_dir };
    path /= build_conf.filename;

    if (!std::filesystem::exists(path)) {
        if (auto err = gen::create_make_file()) {
            err.report();
            return;
        }

        std::filesystem::path tram_dir { fs::TRAM_DIR };

        if (std::filesystem::exists(tram_dir / "Makefile"))
            system::call(std::format("make -f {}/Makefile config={}", fs::TRAM_DIR, build_config));
        else
            std::cout << "No Makfile was found in " << (tram_dir / "Makefile") << std::endl;
    }

    system::call(std::format("{} {}", path.string(), psap::string::join_strings(parser.args(), " ")));
};

inline auto VERSION_ACTION = []([[maybe_unused]] const auto& _parser, [[maybe_unused]] const auto& _cmd) {
    std::cout << "tram v2024.11.1-dp" << std::endl;
};

inline auto LICENCE_ACTION = []([[maybe_unused]] const auto& _parser, [[maybe_unused]] const auto& _cmd) {
    std::cout
        << std::format("\n{}\n{}\n\n{}\n\n\n{}\n{}\n\n{}\n\n\n{}\n{}\n\n{}\n",
               "ArgParser by Paulanerus",
               "https://github.com/Paulanerus/ArgParser",
               psap::LICENSE_NOTICE,
               "toml11 by ToruNiina",
               "https://github.com/ToruNiina/toml11",
               toml::license_notice(),
               "libcurl by Daniel Stenberg",
               "https://curl.se/libcurl/",
               "https://curl.se/docs/copyright.html")
        << std::endl;
};
}
