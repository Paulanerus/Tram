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
        static TramConfig create(std::string &&project_name, ProjectKind &&kind, CppVersion &&cpp_version)
        {
            return TramConfig{ProjectInfo{std::move(project_name), "0.0.1", std::move(kind), std::move(cpp_version)}, {}, {}};
        }

        static std::optional<TramConfig> load()
        {
            if (!std::filesystem::exists(s_ConfigFilename))
                return {};

            try
            {
                const auto config = toml::parse(s_ConfigFilename);

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

        bool save() const
        {
            std::ofstream file_out{"Tram.toml", std::ios::trunc};

            if (!file_out.is_open())
                return false;

            toml::value dependencies{
                {"dependencies", {{"links", m_Links}, {"libs", m_Libraries}}},
            };

            file_out << m_ProjectInfo.into_toml() << dependencies;

            return true;
        }

        ProjectInfo &info()
        {
            return m_ProjectInfo;
        }

        std::vector<std::string> &links()
        {
            return m_Links;
        }

        std::vector<std::string> &libraries()
        {
            return m_Libraries;
        }

    private:
        inline const static std::string s_ConfigFilename{"Tram.toml"};

        mutable ProjectInfo m_ProjectInfo;

        mutable StringVec m_Links;

        mutable StringVec m_Libraries;

        TramConfig(ProjectInfo &&info, StringVec &&links, StringVec &&libs) noexcept : m_ProjectInfo(std::move(info)), m_Links(std::move(links)), m_Libraries(std::move(libs)){};
    };
}