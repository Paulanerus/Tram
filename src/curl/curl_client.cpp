#include "curl_client.hpp"

#include <format>
#include <fstream>

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

        std::size_t write_data(void* buffer, std::size_t size, std::size_t nmemb, void* stream) noexcept
        {
            if (!buffer || !stream)
                return 0;

            auto out = static_cast<std::ofstream*>(stream);

            std::size_t nbytes = size * nmemb;
            out->write(static_cast<char*>(buffer), nbytes);

            return out->good() ? nbytes : 0;
        }
    }

    TramError Client::download_file(std::string_view name, std::string_view url) noexcept
    {
        std::ofstream file_out { name.data(), std::ios::trunc };

        if (!file_out.is_open())
            return make_error(ErrorCode::UnableToCreateFile, "Could not create download file.");

        curl_easy_setopt(m_Handle.get(), CURLOPT_URL, url.data());

        curl_easy_setopt(m_Handle.get(), CURLOPT_WRITEFUNCTION, internal::write_data);
        curl_easy_setopt(m_Handle.get(), CURLOPT_WRITEDATA, &file_out);

        auto res = curl_easy_perform(m_Handle.get());

        if (res != CURLE_OK)
            return make_error(ErrorCode::Curl, std::format("File download from '{}' failed. Error code: {}", url, static_cast<std::underlying_type<CURLcode>::type>(res)));

        return NO_ERROR;
    }
}
}
