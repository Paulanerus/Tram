#pragma once

#include <toml.hpp>

#include <array>
#include <string>
#include <string_view>
#include <vector>

namespace tram {
namespace internal {
    struct settings_conf {
        std::string name;
        std::string version;
    };

    struct build_filter_conf {
        std::string name;
        std::vector<std::string> defines;
        bool symbols;
        bool optimize;

        void from_toml(const toml::value& v);
    };

    struct build_conf {
        std::string kind;
        std::string lang;
        std::string arch;
        std::vector<std::string> src_files {};
        std::vector<std::string> include_files {};
        std::string filename;
        std::string warning;
        std::vector<build_filter_conf> configurations {};

        void from_toml(const toml::value& v);
    };

    inline constexpr std::array<std::string_view, 3> ALLOWED_KIND_VALUES = { "App", "Shared", "Static" };

    inline constexpr std::array<std::string_view, 2> ALLOWED_ARCH_VALUES = { "32", "64" };

    inline constexpr std::array<std::string_view, 3> ALLOWED_WARNING_VALUES = { "Off", "Default", "Extra" };

    struct library {
        std::string name;
        std::string url;
        std::string branch;
        std::string include_files;
        std::string src_files;
        std::string kind;
        std::vector<std::string> links;
        std::vector<std::string> defines;

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
