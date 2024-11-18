#pragma once

#include "../error/error.hpp"

#include <curl/curl.h>

#include <fstream>
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

        std::size_t write_data(void* buffer, std::size_t size, std::size_t nmemb, void* stream) noexcept;
    }

    class Client {
    public:
        Client(bool verbose = false) noexcept
            : m_Handle(curl_easy_init(), &curl_easy_cleanup)
            , m_Failed(!m_Handle)
        {
            if (m_Failed)
                return;

            curl_easy_setopt(m_Handle.get(), CURLOPT_VERBOSE, verbose ? 1 : 0);
            curl_easy_setopt(m_Handle.get(), CURLOPT_NOPROGRESS, 1);
            curl_easy_setopt(m_Handle.get(), CURLOPT_FOLLOWLOCATION, 1);
        }

        TramError download_file(std::string_view name, std::string_view url) noexcept;

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
