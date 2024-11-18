#include "lib_manager.hpp"

#include "../utility/utility.hpp"

#include <filesystem>
#include <format>
#include <system_error>

namespace tram {
namespace lib {

    internal::lib_validate_result validate_install(const tram::internal::library& lib) noexcept
    {
        const std::filesystem::path lib_path { fs::TRAM_LIBS };

        std::error_code ec;

        auto exists = std::filesystem::exists(lib_path / lib.name, ec);

        if (ec) {
            auto err = make_systemerror(ec);
            err.report();

            return { false, false };
        }

        if (exists)
            return { true, false };

        auto global_dir_path = fs::global_tram_dir();

        exists = std::filesystem::exists(global_dir_path / lib.name, ec);

        if (ec) {
            auto err = make_systemerror(ec);
            err.report();

            return { false, false };
        }

        return { exists, exists };
    }

    void install_lib([[maybe_unused]] const tram::internal::library& lib, [[maybe_unused]] bool global) noexcept
    {
    }

    TramError remove_lib(const tram::internal::library& lib, bool global) noexcept
    {
        const std::filesystem::path lib_path { fs::TRAM_LIBS };

        auto [installed, in_global_scope] = lib::validate_install(lib);

        if (!installed)
            return make_error(ErrorCode::LibraryIsNotInstalled, std::format("The library '{}' is not installed and cannot be deleted.", lib.name));

        std::error_code ec;

        if (in_global_scope) {
            if (!global)
                return make_error(ErrorCode::GlobalLibraryCannotBeRemoved, std::format("Library '{}' is in the global scope and remove global libraries was not allowed.", lib.name));

            std::filesystem::remove_all(fs::global_tram_dir() / lib.name, ec);
        } else
            std::filesystem::remove_all(lib_path / lib.name, ec);

        return ec ? make_systemerror(ec) : NO_ERROR;
    }
}
}
