#pragma once

#include <toml.hpp>

#include <array>
#include <string>
#include <string_view>
#include <vector>

namespace tram {
namespace internal {
    struct settings_conf {
        std::string name, version;
    };

    struct build_filter_conf {
        std::string name;
        std::vector<std::string> defines;
        bool symbols, optimize;

        void from_toml(const toml::value& v);
    };

    struct build_conf {
        std::string kind { "app" }, lang { "c++11" }, arch { "64" }, filename { "my_app" }, out { "bin" }, warning {}, toolset { "gcc" };
        std::vector<std::string> src_files {}, include_files {};
        std::vector<build_filter_conf> configurations {};

        std::string default_config() const noexcept;

        std::string resolve_build_config(const std::optional<std::string>& val) const noexcept;

        void from_toml(const toml::value& v);
    };

    inline constexpr std::array<std::string_view, 3> ALLOWED_KIND_VALUES = { "app", "shared", "static" };

    inline constexpr std::array<std::string_view, 3> ALLOWED_WARNING_VALUES = { "off", "default", "extra" };

    struct library_build_cmd {
        std::string windows {}, linux {}, mac {};
    };

    struct library {
        std::string name, url, branch, include_files, src_files, kind;
        std::vector<std::string> links {}, defines {};

        library_build_cmd build_cmd {};

        void from_toml(const toml::value& v);
    };

    struct wo_comment_config {
        using comment_type = toml::discard_comments;

        using boolean_type = bool;
        using integer_type = std::int64_t;
        using floating_type = double;
        using string_type = std::string;

        template<typename T>
        using array_type = std::vector<T>;
        template<typename K, typename T>
        using table_type = std::unordered_map<K, T>;

        static toml::result<integer_type, toml::error_info>
        parse_int(const std::string& str, const toml::source_location src, const std::uint8_t base)
        {
            return toml::read_int<integer_type>(str, src, base);
        }

        static toml::result<floating_type, toml::error_info>
        parse_float(const std::string& str, const toml::source_location src, const bool is_hex)
        {
            return toml::read_float<floating_type>(str, src, is_hex);
        }
    };

    class config_loader {
    public:
        config_loader() noexcept = default;

        void load();

        const settings_conf& settings() const noexcept
        {
            return m_Settings;
        }

        const build_conf& build() const noexcept
        {
            return m_Build;
        }

        const std::vector<build_filter_conf>& build_configurations() const noexcept
        {
            return m_Build.configurations;
        }

        const std::vector<internal::library>& libraries() const noexcept
        {
            return m_Libraries;
        }

    private:
        settings_conf m_Settings;

        build_conf m_Build;

        std::vector<internal::library> m_Libraries;

        void resolve_placeholder();
    };
}

inline internal::config_loader& config()
{
    static internal::config_loader config;

    return config;
}

inline void load_config() noexcept
{
    config().load();
}
}
