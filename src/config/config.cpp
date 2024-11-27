#include "config.hpp"

#include "../utility/utility.hpp"
#include "toml11/find.hpp"
#include <arg_parser.hpp>

#include <iostream>

namespace tram {
namespace internal {

    void build_filter_conf::from_toml(const toml::value& v)
    {
        this->defines = toml::find_or<std::vector<std::string>>(v, "defines", {});
        this->symbols = toml::find_or<bool>(v, "symbols", true);
        this->optimize = toml::find_or<bool>(v, "optimize", false);
    }

    std::string build_conf::default_config() const noexcept
    {
        return configurations.empty() ? "" : configurations[0].name;
    }

    std::string build_conf::resolve_build_config(const std::optional<std::string>& val) const noexcept
    {
        if (configurations.empty() || !val.has_value())
            return default_config();

        for (auto& conf : configurations) {

            if (conf.name == val.value())
                return conf.name;
        }

        return default_config();
    }

    void build_conf::from_toml(const toml::value& v)
    {
        this->kind = toml::find_or(v, "kind", "app");

        psap::string::copy_str_to_lower(kind);

        if (!util::contains_value(ALLOWED_KIND_VALUES, kind))
            kind = "app";

        this->lang = toml::find_or(v, "lang", "c++11");
        psap::string::convert_str_to_lower(lang);

        this->arch = toml::find_or(v, "arch", "64");

        if (arch != "32" && arch != "64")
            arch = "64";

        this->src_files = toml::find_or(v, "src_files", std::vector<std::string> {});
        this->include_files = toml::find_or(v, "include_files", std::vector<std::string> {});
        this->filename = toml::find_or(v, "filename", "my_app");
        this->out = toml::find_or(v, "out", "bin");

        this->warning = toml::find_or(v, "warning", "default");

        psap::string::convert_str_to_lower(warning);

        if (!util::contains_value(ALLOWED_WARNING_VALUES, warning))
            warning = "default";

        this->toolset = toml::find_or(v, "toolset", "gcc");

        psap::string::convert_str_to_lower(toolset);

        if (toolset != "gcc" && toolset != "clang")
            toolset = "gcc";

        for (const auto& [key, value] : v.as_table()) {
            if (!value.is_table())
                continue;

            auto conf = toml::find<internal::build_filter_conf>(v, key);
            conf.name = key;

            configurations.emplace_back(std::move(conf));
        }
    }

    void library::from_toml(const toml::value& v)
    {
        this->url = toml::find_or(v, "url", "");
        this->branch = toml::find_or(v, "branch", "");
        this->src_files = toml::find_or(v, "src_files", "");
        this->include_files = toml::find_or(v, "include_files", "");
        this->kind = toml::find_or(v, "kind", "");

        this->build_cmd.windows = toml::find_or(v, "cmd", "windows", "");
        this->build_cmd.linux = toml::find_or(v, "cmd", "linux", "");
        this->build_cmd.mac = toml::find_or(v, "cmd", "mac", "");

        this->links = toml::find_or<std::vector<std::string>>(v, "links", {});
        this->defines = toml::find_or<std::vector<std::string>>(v, "defines", {});
    }

    void config_loader::load()
    {
        const auto& parse_result = toml::try_parse<wo_comment_config>(fs::TRAM_PROJECT_FILE.data());

        if (parse_result.is_err())
            std::cout << parse_result.as_err()[0] << std::endl;
        else {
            auto& result = parse_result.as_ok();

            m_Settings.name = toml::find_or<std::string>(result, "name", "dummy");
            m_Settings.version = toml::find_or<std::string>(result, "version", "0.0.1");

            m_Build = toml::find_or<internal::build_conf>(result, "build", internal::build_conf {});

            auto libs = toml::find_or(result, "lib", toml::table {});

            for (const auto& [key, val] : libs) {
                if (!val.is_table())
                    continue;

                auto lib = toml::get<internal::library>(val);
                lib.name = key;

                m_Libraries.emplace_back(std::move(lib));
            }

            resolve_placeholder();
        }
    }

    void config_loader::add_library([[maybe_unused]] internal::library&& lib, [[maybe_unused]] bool global)
    {
        std::cout << "TODO: not yet implemented" << std::endl;
    }

    void config_loader::resolve_placeholder()
    {
        static const std::unordered_map<std::string_view, std::string_view> placeholders = {
            { "$name", m_Settings.name },
            { "$ver", m_Settings.version },
            { "$arch", m_Build.arch },
            { "$build", "$(config)" },
            { " ", "_" }
        };

        for (auto& [key, val] : placeholders) {
            string::replace_all(m_Build.out, key, val);
            string::replace_all(m_Build.filename, key, val);
        }
    }
}
}
