#pragma once

#include <cstdlib>
#include <string_view>

namespace tram {
namespace util {
    inline int system_call(std::string_view command) noexcept
    {
        int status_code = std::system(command.data());

        return status_code;
    }
}
}
