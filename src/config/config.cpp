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

    void build_conf::from_toml(const toml::value& v)
    {
        this->kind = toml::find_or(v, "kind", "app");

        psap::string::copy_str_to_lower(kind);

        if (!util::contains_value(ALLOWED_KIND_VALUES, kind))
            kind = "app";

        this->lang = toml::find_or(v, "lang", "C++11");
        psap::string::convert_str_to_lower(lang);

        this->arch = toml::find_or(v, "arch", "64");

        if (arch != "32" && arch != "64")
            arch = "64";

        this->src_files = toml::find_or(v, "src_files", std::vector<std::string> {});
        this->include_files = toml::find_or(v, "include_files", std::vector<std::string> {});
        this->filename = toml::find_or(v, "filename", "my_app");

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
        this->branch = toml::find_or(v, "branch", "master");
        this->src_files = toml::find_or(v, "src_files", "");
        this->include_files = toml::find_or(v, "include_files", "");
        this->kind = toml::find_or(v, "kind", "Shared");

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

            m_Build = toml::find_or<internal::build_conf>(result, "build", internal::build_conf { .kind = "app", .lang = "c++11", .arch = "64", .filename = "my_app", .warning = "", .toolset = "gcc" });

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
