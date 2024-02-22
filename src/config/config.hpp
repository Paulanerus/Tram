#pragma once

#include "../premake/configurations.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <optional>
#include <filesystem>

#include <toml.hpp>

using StringVec = std::vector<std::string>;

namespace Tram
{
    struct ProjectInfo
    {
        std::string project_name;

        std::string version;

        ProjectKind kind;

        CppVersion cpp_version;

        void from_toml(const toml::value &v)
        {
            this->project_name = toml::find<std::string>(v, "name");
            this->version = toml::find<std::string>(v, "version");
            this->cpp_version = static_cast<CppVersion>(toml::find<uint16_t>(v, "cpp_version"));
            this->kind = static_cast<ProjectKind>(toml::find<uint8_t>(v, "kind"));
        }

        toml::value into_toml() const
        {
            return toml::value{
                {"project", {{"cpp_version", static_cast<std::underlying_type<CppVersion>::type>(this->cpp_version)}, {"kind", static_cast<std::underlying_type<ProjectKind>::type>(this->kind)}, {"version", this->version}, {"name", this->project_name}}},
            };
        }
    };

    class TramConfig
    {
    public:
        static std::optional<TramConfig> load()
        {
            const std::string config_filename{"example.toml"};

            if (!std::filesystem::exists(config_filename))
                return {};

            try
            {
                const auto config = toml::parse(config_filename);

                auto info = toml::find<ProjectInfo>(config, "project");

                auto links = toml::find_or<StringVec>(config, "dependencies", "links", StringVec{});

                auto libs = toml::find_or<StringVec>(config, "dependencies", "libs", StringVec{});

                return TramConfig{std::move(info), std::move(links), std::move(libs)};
            }
            catch (toml::syntax_error &syntax_error)
            {
                std::cout << syntax_error.what() << std::endl;
            }

            return {};
        }

        ProjectInfo info() const
        {
            return m_ProjectInfo;
        }

        std::vector<std::string> links() const
        {
            return m_Links;
        }

        std::vector<std::string> libraries() const
        {
            return m_Libs;
        }

    private:
        const ProjectInfo m_ProjectInfo;

        const StringVec m_Links;

        const StringVec m_Libs;

        TramConfig(ProjectInfo &&info, StringVec &&links, StringVec &&libs) noexcept : m_ProjectInfo(std::move(info)), m_Links(std::move(links)), m_Libs(std::move(libs)){};
    };
}