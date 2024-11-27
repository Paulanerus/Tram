#pragma once

#include "../config/config.hpp"
#include "../curl/curl_client.hpp"
#include "../error/error.hpp"

namespace tram {
namespace lib {
    enum class InstallLocation {
        None = 0,
        Local = 1,
        Global = 2,
        Lib = 3,
        Lib32 = 4
    };

    namespace internal {
        std::string resolve_url(const std::string& url, const std::string& branch) noexcept;
    }

    InstallLocation validate_install(const tram::internal::library& lib) noexcept;

    TramError install_lib(curl::Client& client, const tram::internal::library& lib, bool global) noexcept;

    TramError remove_lib(const tram::internal::library& lib, bool global) noexcept;
}
}
