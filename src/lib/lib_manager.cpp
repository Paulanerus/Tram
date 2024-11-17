#include "lib_manager.hpp"

#include "../error/error.hpp"
#include "../utility/utility.hpp"

#include <filesystem>
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

    void remove_lib([[maybe_unused]] const tram::internal::library& lib) noexcept
    {
    }
}
}
