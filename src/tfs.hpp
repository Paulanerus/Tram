#pragma once

#include "error.hpp"

#include <unordered_set>
#include <system_error>
#include <string_view>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <array>

namespace tram
{
    namespace fs
    {
        inline constexpr std::array<std::string_view, 8> C_CPP_EXTENSIONS = {".c", ".C", ".cc", ".cpp", ".CPP", ".cp", ".c++", ".cxx"};

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

            ret = std::filesystem::create_directories(path, ec);

            if (ec)
                return make_systemerror(ec);

            return ret ? NO_ERROR : make_error(ErrorCode::DirAlreadyExists, "Directory already exists.");
        }

        inline TramError create_empty_file(const std::filesystem::path &path, bool override = false) noexcept
        {
            std::ofstream file{path, override ? std::ios::trunc : std::ios::app};

            if (file.fail())
                return make_error(ErrorCode::UnableToCreateFile, "Could not create file.");

            return NO_ERROR;
        }

        inline bool is_src_file(const std::filesystem::path &file_path)
        {
            if (!std::filesystem::is_regular_file(file_path))
                return false;

            for (auto &ext : C_CPP_EXTENSIONS)
            {
                if (file_path.extension() == ext)
                    return true;
            }

            return false;
        }

        inline std::unordered_set<std::filesystem::path> collect_src_files(const std::vector<std::filesystem::path> &paths)
        {
            std::unordered_set<std::filesystem::path> src_files;

            for (auto &path : paths)
            {
                if (std::filesystem::is_directory(path))
                {
                    for (auto &entry : std::filesystem::recursive_directory_iterator(path))
                    {
                        if (is_src_file(entry.path()))
                            src_files.insert(entry.path());
                    }
                }

                if (is_src_file(path))
                    src_files.insert(path);
            }

            return src_files;
        }
    }
}