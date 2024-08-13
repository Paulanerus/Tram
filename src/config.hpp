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

            std::string src_files;
            std::string include_files;

            std::string filename;

            std::string warning;

            std::vector<build_filter_conf> configurations;

            void from_toml(const toml::value &v)
            {
                this->kind = toml::find_or(v, "kind", "App");
                this->src_files = toml::find_or(v, "src_files", "");
                this->include_files = toml::find_or(v, "include_files", "");
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

        struct dependency
        {
            std::string include_path;
            std::string branch;
            bool compile;
            bool link;
        };

        class config_loader
        {
        public:
            config_loader() noexcept = default;

            void load() noexcept
            {
                const auto &parse_result = toml::try_parse("example.tram.toml");

                if (parse_result.is_err())
                    std::cout << parse_result.as_err()[0] << std::endl;
                else
                {
                    auto &result = parse_result.as_ok();

                    m_Settings.name = toml::find_or<std::string>(result, "name", "dummy");
                    m_Settings.version = toml::find_or<std::string>(result, "version", "0.0.1");

                    m_Build = toml::find<internal::build_conf>(result, "build");

                    // TODO (paul) add dependency loading
                }
            }

            settings_conf &settings() noexcept
            {
                return m_Settings;
            }

            build_conf &build() noexcept
            {
                return m_Build;
            }

            const std::vector<internal::dependency> &dependencies() const noexcept
            {
                return m_Dependencies;
            }

        private:
            settings_conf m_Settings;

            build_conf m_Build;

            std::vector<internal::dependency> m_Dependencies;
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