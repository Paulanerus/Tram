#pragma once

#include "../config/config.hpp"

namespace tram {
namespace lib {

    bool validate_install(const tram::internal::library& lib) noexcept;

    void install_lib(const tram::internal::library& lib) noexcept;

    void remove_lib(const tram::internal::library& lib) noexcept;
}
}
