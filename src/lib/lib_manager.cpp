#include "lib_manager.hpp"

#include "../utility/utility.hpp"

#include <cstdlib>
#include <filesystem>
#include <format>
#include <system_error>
#include <type_traits>
#include <miniz/miniz.h>

namespace tram {
namespace lib {

    namespace internal {
        std::string resolve_url(const std::string& url, const std::string& branch) noexcept
        {
            auto it = url.find("@");

            if (it == std::string::npos)
                return url;

            auto shortcut = url.substr(0, it);

            if (shortcut == "gh" || shortcut == "github")
                return std::format("https://api.github.com/repos/{}/zipball/{}", url.substr(it + 1), branch);
            else if (shortcut == "system")
                return url.substr(it + 1);
            else
                return url;
        }
    }

    TramError unpack_zipfile(const std::filesystem::path& archive_path, [[maybe_unused]] const std::filesystem::path& out_path) noexcept
    {
        if (!archive_path.has_parent_path())
            return make_error(ErrorCode::Miniz, "'archive_path' has no parent directory");

        auto parent_path = archive_path.parent_path();

        mz_zip_archive archive {};
        if (!mz_zip_reader_init_file(&archive, archive_path.c_str(), 0))
            return make_error(ErrorCode::Miniz, std::format("Failed to init zip reader ({})", mz_zip_get_error_string(archive.m_last_error)));

        auto num_files = mz_zip_reader_get_num_files(&archive);
        for (mz_uint i {}; i < num_files; i++) {
            mz_zip_archive_file_stat file_stat;

            if (!mz_zip_reader_file_stat(&archive, i, &file_stat)) {
                make_error(ErrorCode::Miniz, std::format("Failed to read file info ({})", mz_zip_get_error_string(archive.m_last_error))).report();
                continue;
            }

            if (mz_zip_reader_is_file_a_directory(&archive, i)) {
                if (auto err = fs::create_dir_if_notexists(parent_path / file_stat.m_filename))
                    err.report();
            } else {
                if (!mz_zip_reader_extract_to_file(&archive, i, (parent_path / file_stat.m_filename).c_str(), 0)) {
                    make_error(ErrorCode::Miniz, std::format("Failed file extraction ({})", mz_zip_get_error_string(archive.m_last_error))).report();
                    continue;
                }
            }
        }

        // FIXME: Rename root directory to 'out_path'.

        return mz_zip_reader_end(&archive) ? NO_ERROR : make_error(ErrorCode::Miniz, std::format("Failed to close zip reader ({})", mz_zip_get_error_string(archive.m_last_error)));
    }

    InstallLocation validate_install(const tram::internal::library& lib) noexcept
    {
        auto url = internal::resolve_url(lib.url, lib.branch);

        if (!url.starts_with("http")) {

            auto current_os = system::current_os();

            if (current_os == system::OS::Windows || current_os == system::OS::Unknown)
                return InstallLocation::None;

            auto lib_file_name = "lib" + lib.url;

            for (auto& entry : std::filesystem::recursive_directory_iterator("/usr/lib")) {
                if (!entry.is_regular_file())
                    continue;

                auto name = entry.path().filename().string();

                if (name.starts_with(lib_file_name))
                    return InstallLocation::Lib;
            }

            for (auto& entry : std::filesystem::recursive_directory_iterator("/usr/lib32")) {
                if (!entry.is_regular_file())
                    continue;

                auto name = entry.path().filename().string();

                if (name.starts_with(lib_file_name))
                    return InstallLocation::Lib32;
            }

            return InstallLocation::None;
        }

        const std::filesystem::path base_path { fs::TRAM_DIR };

        std::error_code ec;
        auto exists = std::filesystem::exists(base_path / fs::TRAM_LIBS / lib.name, ec);

        if (ec) {
            auto err = make_systemerror(ec);
            err.report();

            return InstallLocation::None;
        }

        if (exists)
            return InstallLocation::Local;

        exists = std::filesystem::exists(fs::global_tram_dir() / fs::TRAM_LIBS / lib.name, ec);

        if (ec) {
            auto err = make_systemerror(ec);
            err.report();

            return InstallLocation::None;
        }

        return exists ? InstallLocation::Global : InstallLocation::None;
    }

    TramError install_lib(curl::Client& client, const tram::internal::library& lib, bool global) noexcept
    {
        InstallLocation loc = validate_install(lib);

        if ((static_cast<std::underlying_type_t<InstallLocation>>(loc) >= 3) || (loc == InstallLocation::Global && global) || (loc == InstallLocation::Local && !global))
            return make_error(ErrorCode::LibraryIsAlreadyInstalled);

        auto base_path = global ? fs::global_tram_dir() : std::filesystem::path(fs::TRAM_DIR);
        base_path /= fs::TRAM_LIBS;

        if (auto err = fs::create_dir_if_notexists(base_path); err && !err.is(ErrorCode::DirAlreadyExists))
            return err;

        std::string url = internal::resolve_url(lib.url, lib.branch);

        if (url.starts_with("http")) {
            auto zip_path = base_path / std::format("{}.zip", lib.name);

            if (auto err = client.download_file(zip_path, url))
                return err;

            if (auto err = unpack_zipfile(zip_path, base_path / lib.name))
                return err;
        }

        return NO_ERROR;
    }

    TramError remove_lib(const tram::internal::library& lib, bool global) noexcept
    {
        const std::filesystem::path base_path { fs::TRAM_DIR };

        InstallLocation loc = lib::validate_install(lib);

        if (static_cast<std::underlying_type_t<InstallLocation>>(loc) >= 3)
            return NO_ERROR;
        else if (loc == InstallLocation::None)
            return make_error(ErrorCode::LibraryIsNotInstalled, std::format("The library '{}' is not installed and cannot be deleted.", lib.name));

        std::error_code ec;
        if (loc == InstallLocation::Global) {
            if (!global)
                return make_error(ErrorCode::GlobalLibraryCannotBeRemoved, std::format("Library '{}' is in the global scope and remove global libraries was not allowed.", lib.name));

            std::filesystem::remove_all(fs::global_tram_dir() / fs::TRAM_LIBS / lib.name, ec);
        } else
            std::filesystem::remove_all(base_path / fs::TRAM_LIBS / lib.name, ec);

        return ec ? make_systemerror(ec) : NO_ERROR;
    }
}
}
