#pragma once

#include "../config/config.hpp"
#include "../error/error.hpp"

namespace tram {
namespace lib {
    namespace internal {
        struct lib_validate_result {
            bool installed;
            bool in_global_scope;
        };
    }

    internal::lib_validate_result validate_install(const tram::internal::library& lib) noexcept;

    void install_lib(const tram::internal::library& lib, bool global) noexcept;

    TramError remove_lib(const tram::internal::library& lib, bool global) noexcept;
}
}
