#pragma once

#include "../error.hpp"

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

namespace fs {
    inline constexpr std::array<std::string_view, 8> C_CPP_EXTENSIONS = { ".c", ".C", ".cc", ".cpp", ".CPP", ".cp", ".c++", ".cxx" };

    inline constexpr std::string_view TRAM_TEMP = ".tram/";

    inline constexpr std::string_view TRAM_BUILD = ".tram/build/";

    inline constexpr std::string_view TRAM_PROJECT_FILE = "tram.toml";

    TramError create_dir_if_notexists(const std::filesystem::path& path) noexcept;

    TramError create_empty_file(const std::filesystem::path& path, bool override = false) noexcept;

    bool is_src_file(const std::filesystem::path& file_path);

    std::unordered_set<std::filesystem::path> collect_src_files(const std::vector<std::string>& paths);
}
}
