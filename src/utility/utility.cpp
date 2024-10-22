#include "utility.hpp"
#include <fstream>
#include <iostream>
#include <system_error>

namespace tram {
namespace system {
    int call(std::string_view command) noexcept
    {
        int status_code = std::system(command.data());

        return status_code;
    }
}

namespace fs {
    TramError create_dir_if_notexists(const std::filesystem::path& path) noexcept
    {
        std::error_code ec;
        bool ret = std::filesystem::create_directories(path, ec);

        if (ec)
            return make_systemerror(ec);

        return ret ? NO_ERROR : make_error(ErrorCode::DirAlreadyExists, "Directory already exists.");
    }

    TramError create_file(const std::filesystem::path& path, std::string_view content, bool override) noexcept
    {
        std::ofstream file { path, override ? std::ios::trunc : std::ios::app };

        if (file.fail())
            return make_error(ErrorCode::UnableToCreateFile, "Could not create file.");

        file << content;

        return NO_ERROR;
    }

    bool is_src_file(const std::filesystem::path& file_path)
    {
        if (!std::filesystem::is_regular_file(file_path))
            return false;

        for (auto& ext : C_CPP_EXTENSIONS) {
            if (file_path.extension() == ext)
                return true;
        }

        return false;
    }

    std::unordered_set<std::filesystem::path> collect_src_files(const std::vector<std::string>& paths)
    {
        std::unordered_set<std::filesystem::path> src_files;

        for (auto& path : paths) {
            if (std::filesystem::is_directory(path)) {
                for (auto& entry : std::filesystem::recursive_directory_iterator(path)) {
                    if (is_src_file(entry.path()))
                        src_files.insert(entry.path());
                }
            }

            if (is_src_file(path))
                src_files.insert(path);
        }

        return src_files;
    }

    void create_sample_files(const std::filesystem::path& path) noexcept
    {
        for (auto& file : fs::SAMPLE_FILES) {

            if (file.path.ends_with("/")) {

                if (auto err = fs::create_dir_if_notexists(path / file.path); !err.is(ErrorCode::DirAlreadyExists))
                    err.report();
            } else {

                if (auto err = fs::create_file(path / file.path, file.content))
                    err.report();
            }
        }
    }
}
}
