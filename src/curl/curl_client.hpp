#pragma once

#include "../error/error.hpp"

#include <curl/curl.h>

#include <memory>
#include <string_view>

namespace tram {
namespace curl {

    namespace internal {
        class global_curl_state {
        public:
            global_curl_state() noexcept;
            ~global_curl_state() noexcept;
        };
    }

    class Client {
    public:
        Client(bool verbose)
            : m_Handle(curl_easy_init(), &curl_easy_cleanup)
        {
            if (!m_Handle) {
                m_Failed = true;
                return;
            }

            set_opt(CURLOPT_VERBOSE, verbose ? 1 : 0);
            set_opt(CURLOPT_NOPROGRESS, 1);
        }

        template<typename T>
        void set_opt(CURLoption option, T val) noexcept
        {
            curl_easy_setopt(m_Handle.get(), option, val);
        }

        bool failed() const noexcept
        {
            return m_Failed;
        }

    private:
        const std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> m_Handle;

        bool m_Failed {};
    };

    inline void global_init()
    {
        static internal::global_curl_state global_state;
    }
}
}
