#include "lib_manager.hpp"

#include "../utility/utility.hpp"

#include <filesystem>
#include <format>
#include <system_error>

namespace tram {
namespace lib {

    bool validate_install(const tram::internal::library& lib) noexcept
    {
        const std::filesystem::path lib_path { fs::TRAM_LIBS };

        std::error_code ec;

        auto exists = std::filesystem::exists(lib_path / lib.name, ec);

        if (ec) {
            auto err = make_systemerror(ec);
            err.report();

            return false;
        }

        return exists;
    }

    void install_lib([[maybe_unused]] const tram::internal::library& lib) noexcept
    {
    }

    TramError remove_lib(const tram::internal::library& lib) noexcept
    {
        const std::filesystem::path lib_path { fs::TRAM_LIBS };

        if (!validate_install(lib))
            return make_error(ErrorCode::LibraryIsNotInstalled, std::format("The library '{}' is not installed and cannot be deleted.", lib.name));

        std::error_code ec;

        std::filesystem::remove_all(lib_path / lib.name, ec);

        return ec ? make_systemerror(ec) : NO_ERROR;
    }
}
}
