#pragma once

#include "error.hpp"

#include <system_error>
#include <string_view>
#include <filesystem>
#include <iostream>
#include <fstream>

namespace tram
{
    namespace fs
    {
        inline constexpr std::string_view TRAM_TEMP = ".tram/";

        inline constexpr std::string_view TRAM_BUILD = ".tram/build/";

        inline constexpr std::string_view TRAM_PROJECT_FILE = "tram.toml";

        inline TramError create_dir_if_notexists(const std::filesystem::path &path) noexcept
        {
            std::error_code ec;

            bool ret{};

            ret = std::filesystem::exists(TRAM_PROJECT_FILE, ec);

            if (ec)
                return make_systemerror(ec);

            if (!ret)
                return make_error(ErrorCode::MissingConfigFile, "The default tram config file is missing.");

            ret = std::filesystem::create_directory(path, ec);

            if (ec)
                return make_systemerror(ec);

            return ret ? NO_ERROR : make_error(ErrorCode::DirAlreadyExists, "Directory already exists.");
        }

        inline TramError create_empty_file(const std::filesystem::path &path, bool override = false) noexcept
        {
            std::ofstream file { path, override ? std::ios::trunc : std::ios::app};

            if (file.fail())
                return make_error(ErrorCode::UnableToCreateFile, "Could not create file.");

            return NO_ERROR;
        }
    }
}