#include "curl_client.hpp"

#include <iostream>

namespace tram {
namespace curl {
    namespace internal {

        global_curl_state::global_curl_state() noexcept
        {
            curl_global_init(CURL_GLOBAL_ALL);
        }

        global_curl_state::~global_curl_state() noexcept
        {
            curl_global_cleanup();
        }
    };
}
}
