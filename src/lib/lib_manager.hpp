#pragma once

#include "../config/config.hpp"
#include "../error/error.hpp"

namespace tram {
namespace lib {

    bool validate_install(const tram::internal::library& lib) noexcept;

    void install_lib(const tram::internal::library& lib) noexcept;

    TramError remove_lib(const tram::internal::library& lib) noexcept;
}
}
