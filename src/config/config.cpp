#include "config.hpp"

#include <iostream>

namespace tram {
namespace internal {

    void build_filter_conf::from_toml(const toml::value& v)
    {
        this->defines = toml::find_or<std::vector<std::string>>(v, "defines", {});
        this->symbols = toml::find_or<bool>(v, "symbols", true);
        this->optimize = toml::find_or<bool>(v, "optimize", false);
    }

    void build_conf::from_toml(const toml::value& v)
    {
        this->kind = toml::find_or(v, "kind", "App");
        this->src_files = toml::find_or(v, "src_files", std::vector<std::string> {});
        this->include_files = toml::find_or(v, "include_files", std::vector<std::string> {});
        this->filename = toml::find_or(v, "filename", "my_app");
        this->warning = toml::find_or(v, "warning", "");

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
        this->branch = toml::find_or(v, "branch", "master");
        this->src_files = toml::find_or(v, "src_files", "");
        this->include_files = toml::find_or(v, "include_files", "");
        this->kind = toml::find_or(v, "kind", "Shared");

        this->links = toml::find_or<std::vector<std::string>>(v, "links", {});
        this->defines = toml::find_or<std::vector<std::string>>(v, "defines", {});
    }

    void config_loader::load()
    {
        const auto& parse_result = toml::try_parse<wo_comment_config>("example/tram.toml");

        if (parse_result.is_err())
            std::cout << parse_result.as_err()[0] << std::endl;
        else {
            auto& result = parse_result.as_ok();

            m_Settings.name = toml::find_or<std::string>(result, "name", "dummy");
            m_Settings.version = toml::find_or<std::string>(result, "version", "0.0.1");

            m_Build = toml::find_or<internal::build_conf>(result, "build", internal::build_conf { .kind = "App", .filename = "my_app" });

            auto libs = toml::find_or(result, "libraries", toml::table {});

            for (const auto& [key, val] : libs) {
                if (!val.is_table())
                    continue;

                auto lib = toml::get<internal::library>(val);
                lib.name = key;

                m_Libraries.emplace_back(std::move(lib));
            }
        }
    }
}
}
