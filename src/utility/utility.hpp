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
    int call(std::string_view command) noexcept;
}

namespace string {
    std::string normalize_filename(std::string filename);
}

namespace fs {

    struct SampleFile {
        std::string_view path;

        std::string_view content { "" };
    };

    inline constexpr std::array<std::string_view, 8> C_CPP_EXTENSIONS = { ".c", ".C", ".cc", ".cpp", ".CPP", ".cp", ".c++", ".cxx" };

    inline constexpr std::array<SampleFile, 5> SAMPLE_FILES = { SampleFile { ".gitignore", ".tram/" }, { "src/" }, { "src/main.cpp" }, { "README.md" }, { "LICENSE.md" } };

    inline constexpr std::string_view TRAM_TEMP = ".tram";

    inline constexpr std::string_view TRAM_BUILD = ".tram/build";

    inline constexpr std::string_view TRAM_LAST_MODIFIED_FILE = ".tram/last_build_file";

    inline constexpr std::string_view TRAM_PROJECT_FILE = "tram.toml";

    TramError create_dir_if_notexists(const std::filesystem::path& path) noexcept;

    TramError create_file(const std::filesystem::path& path, std::string_view content, bool override = false) noexcept;

    bool is_src_file(const std::filesystem::path& file_path);

    std::unordered_set<std::filesystem::path> collect_src_files(const std::vector<std::string>& paths);

    void create_sample_files(const std::filesystem::path& path) noexcept;

    std::size_t last_modified_time(const std::filesystem::path& path) noexcept;

    bool compare_modified_time_from_file(std::size_t time);
}
}
