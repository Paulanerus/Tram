#pragma once

#include <toml.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace tram
{
    namespace internal
    {
        struct settings_conf
        {
            std::string name;
            std::string version;
        };

        struct build_filter_conf
        {
            std::string name;
            std::vector<std::string> defines;
            bool symbols;
            bool optimize;

            void from_toml(const toml::value &v)
            {
                this->defines = toml::find_or<std::vector<std::string>>(v, "defines", {});
                this->symbols = toml::find_or<bool>(v, "symbols", true);
                this->optimize = toml::find_or<bool>(v, "optimize", false);
            }
        };

        struct build_conf
        {
            std::string kind;

            /*
                c
                cpp
                arch
            */

            std::vector<std::string> src_files;
            std::vector<std::string> include_files;

            std::string filename;

            std::string warning;

            std::vector<build_filter_conf> configurations;

            void from_toml(const toml::value &v)
            {
                this->kind = toml::find_or(v, "kind", "App");
                this->src_files = toml::find_or(v, "src_files", std::vector<std::string>{});
                this->include_files = toml::find_or(v, "include_files", std::vector<std::string>{});
                this->filename = toml::find_or(v, "filename", "");
                this->warning = toml::find_or(v, "warning", "");

                for (const auto &[key, value] : v.as_table())
                {
                    if (!value.is_table())
                        continue;

                    auto conf = toml::find<internal::build_filter_conf>(v, key);
                    conf.name = key;

                    configurations.emplace_back(std::move(conf));
                }
            }
        };

        struct library
        {
            std::string name;
            std::string url;
            std::string branch;
            std::string include_files;
            std::string src_files;
            std::string kind;
            std::vector<std::string> links;
            std::vector<std::string> defines;

            void from_toml(const toml::value &v)
            {
                this->url = toml::find_or(v, "url", "");
                this->branch = toml::find_or(v, "branch", "master");
                this->src_files = toml::find_or(v, "src_files", "");
                this->include_files = toml::find_or(v, "include_files", "");
                this->kind = toml::find_or(v, "kind", "Shared");

                this->links = toml::find_or<std::vector<std::string>>(v, "links", {});
                this->defines = toml::find_or<std::vector<std::string>>(v, "defines", {});
            }
        };

        struct wo_comment_config
        {
            using comment_type = toml::discard_comments;

            using boolean_type = bool;
            using integer_type = std::int64_t;
            using floating_type = double;
            using string_type = std::string;

            template <typename T>
            using array_type = std::vector<T>;
            template <typename K, typename T>
            using table_type = std::unordered_map<K, T>;

            static toml::result<integer_type, toml::error_info>
            parse_int(const std::string &str, const toml::source_location src, const std::uint8_t base)
            {
                return toml::read_int<integer_type>(str, src, base);
            }

            static toml::result<floating_type, toml::error_info>
            parse_float(const std::string &str, const toml::source_location src, const bool is_hex)
            {
                return toml::read_float<floating_type>(str, src, is_hex);
            }
        };

        class config_loader
        {
        public:
            config_loader() noexcept = default;

            void load()
            {
                const auto &parse_result = toml::try_parse<wo_comment_config>("example.tram.toml");

                if (parse_result.is_err())
                    std::cout << parse_result.as_err()[0] << std::endl;
                else
                {
                    auto &result = parse_result.as_ok();

                    m_Settings.name = toml::find_or<std::string>(result, "name", "dummy");
                    m_Settings.version = toml::find_or<std::string>(result, "version", "0.0.1");

                    m_Build = toml::find<internal::build_conf>(result, "build");

                    auto libs = toml::find_or(result, "libraries", toml::table{});

                    for (const auto &[key, val] : libs)
                    {
                        if (!val.is_table())
                            continue;

                        auto lib = toml::get<internal::library>(val);
                        lib.name = key;

                        m_Libraries.emplace_back(std::move(lib));
                    }
                }
            }

            const settings_conf &settings() const noexcept
            {
                return m_Settings;
            }

            const build_conf &build() const noexcept
            {
                return m_Build;
            }

            const std::vector<build_filter_conf> &build_configurations() const noexcept
            {
                return m_Build.configurations;
            }

            const std::vector<internal::library> &libraries() const noexcept
            {
                return m_Libraries;
            }

        private:
            settings_conf m_Settings;

            build_conf m_Build;

            std::vector<internal::library> m_Libraries;
        };
    }

    inline internal::config_loader &config()
    {
        static internal::config_loader config;

        return config;
    }

    inline void load_config() noexcept
    {
        config().load();
    }
}