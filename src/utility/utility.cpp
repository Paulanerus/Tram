#include "utility.hpp"

#include <algorithm>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <string>
#include <system_error>

namespace tram {
namespace system {

    OS current_os() noexcept
    {
#ifdef __WIN32
        return OS::Winodws;
#elif __APPLE__
        return OS::Mac;
#elif __unix__
        return OS::UnixBased;
#else
        return OS::Unknown;
#endif
    }

    int call(std::string_view command) noexcept
    {
        int status_code = std::system(command.data());

        return status_code;
    }
}

namespace string {
    std::string normalize_filename(std::string filename)
    {
        std::replace(filename.begin(), filename.end(), ' ', '_');

        return filename;
    }

    void replace_all(std::string& str, std::string_view from, std::string_view to)
    {
        if (from.empty())
            return;

        std::size_t start {};
        while ((start = str.find(from, start)) != std::string::npos) {
            str.replace(start, from.length(), to);
            start += to.length();
        }
    }
};

namespace fs {
    std::filesystem::path global_tram_dir() noexcept
    {
        system::OS os = system::current_os();

        if (os == system::OS::Unknown) {
            return "";
        } else if (os == system::OS::Mac) {
            return "";
        }

        if (auto env = std::getenv(os == system::OS::UnixBased ? "HOME" : "APPDATA")) {

            std::filesystem::path home { env };
            return home / TRAM_TEMP;
        }

        return "";
    }

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

    std::size_t last_modified_time(const std::filesystem::path& path) noexcept
    {
        std::error_code ec;

        auto time = std::filesystem::last_write_time(path, ec);

        return ec ? 0 : time.time_since_epoch().count();
    }

    bool compare_modified_time_from_file(std::size_t time)
    {
        std::ifstream file_in { TRAM_LAST_MODIFIED_FILE.data() };

        if (!file_in.is_open())
            return true;

        std::string line;
        if (!std::getline(file_in, line))
            return true;

        if (line.empty())
            return true;

        try {

            auto num = std::stoull(line);

            return num != time;
        } catch (std::exception& ignored) {
            return true;
        }
    }
}
}
