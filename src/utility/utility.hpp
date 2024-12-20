#pragma once

#include "../error/error.hpp"

#include <array>
#include <cstdlib>
#include <filesystem>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace tram {
namespace system {

    enum class OS {
        Unknown,
        UnixBased,
        Mac,
        Windows,
    };

    OS current_os() noexcept;

    int call(std::string_view command) noexcept;
}

namespace string {
    std::string normalize_filename(std::string filename);

    void replace_all(std::string& str, std::string_view from, std::string_view to);
}

namespace util {

    template<std::size_t N>
    inline bool contains_value(const std::array<std::string_view, N>& values, std::string_view val, bool ignore_case = true)
    {
        auto compare = [ignore_case](std::string_view lhs, std::string_view rhs) {
            if (ignore_case) {
                return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
                    [](char a, char b) { return std::tolower(a) == std::tolower(b); });
            } else {
                return lhs == rhs;
            }
        };

        for (auto& v : values) {
            if (compare(v, val))
                return true;
        }

        return false;
    }
}

namespace fs {

    struct SampleFile {
        std::string_view path;

        std::string_view content { "" };
    };

    inline constexpr std::string_view TRAM_DIR = ".tram";

    inline constexpr std::string_view TRAM_LIBS = "libs";

    inline constexpr std::string_view TRAM_BUILD = ".tram/build";

    inline constexpr std::string_view TRAM_LAST_MODIFIED_FILE = ".tram/last_build_file";

    inline constexpr std::string_view TRAM_PROJECT_FILE = "tram.toml";

    inline constexpr std::array<std::string_view, 8> C_CPP_EXTENSIONS = { ".c", ".C", ".cc", ".cpp", ".CPP", ".cp", ".c++", ".cxx" };

    inline constexpr std::array<SampleFile, 5> SAMPLE_FILES = { SampleFile { ".gitignore", TRAM_DIR }, { "src/" }, { "src/main.cpp", "int main(){}" }, { "README.md" }, { "LICENSE.md" } };

    std::filesystem::path global_tram_dir() noexcept;

    TramError create_dir_if_notexists(const std::filesystem::path& path) noexcept;

    TramError create_file(const std::filesystem::path& path, std::string_view content, bool override = false) noexcept;

    bool is_src_file(const std::filesystem::path& file_path);

    std::unordered_set<std::filesystem::path> collect_src_files(const std::vector<std::string>& paths);

    void create_sample_files(const std::filesystem::path& path) noexcept;

    std::size_t last_modified_time(const std::filesystem::path& path) noexcept;

    bool compare_modified_time_from_file(std::size_t time);
}
}
