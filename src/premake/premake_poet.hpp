#pragma once

#include "configurations.hpp"
#include "../util/strings.hpp"

#include <sstream>
#include <fstream>
#include <string>
#include <filesystem>

using StringVec = std::vector<std::string>;

namespace Tram
{
    class Project;

    class Filter;

    class Script;

    class GenericBuilder
    {
    public:
        GenericBuilder(const std::string &tag, const std::string &value, bool is_child = false) : m_IsChild(is_child)
        {
            m_Content << (is_child ? "\t" : "") << tag << " \"" << value << "\"";
            newLine();
        }

        GenericBuilder(const std::string &tag, const StringVec &value, bool is_child = false) : m_IsChild(is_child)
        {
            m_Content << (is_child ? "\t" : "") << tag << " {\"" << Tram::Util::Join(value, "\", \"") << "\"}";
            newLine();
        }

        GenericBuilder &addChild(const std::string &tag, const std::string &value)
        {
            m_Content << (m_IsChild ? "\t\t" : "\t") << tag << " \"" << value << "\"";
            newLine();

            return *this;
        }

        GenericBuilder &addChild(const std::string &tag, const StringVec &value)
        {
            m_Content << (m_IsChild ? "\t\t" : "\t") << tag << " {\"" << Tram::Util::Join(value, "\", \"") << "\"}";
            newLine();

            return *this;
        }

        GenericBuilder &newLine()
        {
            m_Content << "\n";

            return *this;
        }

        GenericBuilder &append(GenericBuilder &other)
        {
            m_Content << other.build();

            return *this;
        }

        std::string build()
        {
            newLine();
            return m_Content.str();
        }

    private:
        std::stringstream m_Content;

        const bool m_IsChild;
    };

    class FilterBuilder
    {
    public:
        FilterBuilder &configuration(const Configuration &configuration)
        {
            m_Configuration = configuration;

            return *this;
        }

        FilterBuilder &defines(const StringVec &defines)
        {
            m_Defines = defines;

            return *this;
        }

        FilterBuilder &optimize(bool optimize)
        {
            m_Optimize = optimize;

            return *this;
        }

        FilterBuilder &symbols(bool symbols)
        {
            m_Symbols = symbols;

            return *this;
        }

        Filter build();

    private:
        Configuration m_Configuration;

        StringVec m_Defines;

        bool m_Optimize;

        bool m_Symbols;

        FilterBuilder() {}

        friend class Filter;
    };

    class Filter
    {
    public:
        static FilterBuilder Builder()
        {
            return FilterBuilder();
        }

        GenericBuilder &genericBuilder()
        {
            return m_GenericFilterBuilder;
        }

    private:
        GenericBuilder m_GenericFilterBuilder;

        Filter(const Configuration &configuration, const StringVec &defines, bool optimize, bool symbols) : m_GenericFilterBuilder("filter", configuration == Configuration::CONF_DEBUG ? "configurations:Debug" : "configurations:Release", true)
        {
            m_GenericFilterBuilder
                .addChild("defines", defines)
                .addChild("optimize", optimize ? "On" : "Off")
                .addChild("symbols", symbols ? "On" : "Off");
        };

        friend class FilterBuilder;
    };

    class ProjectBuilder
    {
    public:
        ProjectBuilder &name(const std::string &name)
        {
            m_Name = name;

            return *this;
        }

        ProjectBuilder &kind(const ProjectKind &kind)
        {
            m_Kind = kind;

            return *this;
        }

        ProjectBuilder &cppVersion(const CppVersion &cpp_version)
        {
            m_CppVersion = cpp_version;

            return *this;
        }

        ProjectBuilder &language(const Language &language)
        {
            m_Language = language;

            return *this;
        }

        ProjectBuilder &architecture(const Architecture &architecture)
        {
            m_Architecture = architecture;

            return *this;
        }

        ProjectBuilder &targetDir(const std::string &target_dir)
        {
            m_TargetDir = target_dir;

            return *this;
        }

        ProjectBuilder &location(const std::string &location)
        {
            m_Location = location;

            return *this;
        }

        ProjectBuilder &links(const StringVec &links)
        {
            m_Builder.emplace_back("links", links, true);

            return *this;
        }

        ProjectBuilder &libs(const StringVec &libs)
        {
            m_Builder.emplace_back("includedirs", libs, true);

            return *this;
        }

        ProjectBuilder &files(const StringVec &files)
        {
            m_Builder.emplace_back("files", files, true);

            return *this;
        }

        ProjectBuilder &filter(Filter &&filter)
        {
            m_Builder.emplace_back(std::move(filter.genericBuilder()));

            return *this;
        }

        Project build();

    private:
        std::string m_Name;

        ProjectKind m_Kind;

        CppVersion m_CppVersion;

        Language m_Language;

        Architecture m_Architecture;

        std::string m_TargetDir;

        std::string m_Location;

        std::vector<GenericBuilder> m_Builder;

        ProjectBuilder() {}

        friend class Project;
    };

    class Project
    {
    public:
        static ProjectBuilder Builder()
        {
            return ProjectBuilder();
        }

        GenericBuilder &genericBuilder()
        {
            return m_GenericProjectBuilder;
        }

    private:
        GenericBuilder m_GenericProjectBuilder;

        Project(const std::string &name, const ProjectKind &kind, const CppVersion &cpp_version, const Language &language, const Architecture &architecture, const std::string &target_dir, const std::string &location, std::vector<GenericBuilder> &&children) : m_GenericProjectBuilder("project", name)
        {
            m_GenericProjectBuilder
                .addChild("kind", kind == ProjectKind::APP ? "ConsoleApp" : kind == ProjectKind::LIBRARY_DYNAMIC ? "SharedLib"
                                                                                                                 : "StaticLib")
                .addChild("cppdialect", VersionToString(cpp_version))
                .addChild("language", language == Language::C ? "C" : "C++")
                .addChild("architecture", ArchToString(architecture))
                .addChild("targetdir", target_dir)
                .addChild("location", location)
                .newLine();

            std::for_each(children.begin(), children.end(), [&](GenericBuilder &child)
                          { m_GenericProjectBuilder.append(child); });
        };

        friend class ProjectBuilder;
    };

    class ScriptBuilder
    {
    public:
        ScriptBuilder &workspace(const std::string &workspace)
        {
            GenericBuilder workspace_builder{"workspace", workspace};
            workspace_builder.addChild("configurations", StringVec{"Debug", "Release"});

            m_Builder.emplace_back(std::move(workspace_builder));

            return *this;
        }

        ScriptBuilder &project(Project &&project)
        {
            m_Builder.emplace_back(std::move(project.genericBuilder()));

            return *this;
        }

        Script build();

    private:
        ScriptBuilder() noexcept {}

        std::vector<GenericBuilder> m_Builder;

        friend class Script;
    };

    class Script
    {
    public:
        static ScriptBuilder Builder()
        {
            return ScriptBuilder();
        }

        bool saveToFile(const std::filesystem::path &path)
        {
            std::ofstream file_out{path, std::ios::trunc};

            if (!file_out.is_open())
                return false;

            file_out << content.str();

            return file_out.good();
        }

    private:
        Script() {}

        std::stringstream content;

        friend class ScriptBuilder;
    };

    Script ScriptBuilder::build()
    {
        Script script;

        for (auto &builder : m_Builder)
            script.content << builder.build();

        return script;
    }

    Filter FilterBuilder::build()
    {
        return Filter{m_Configuration, m_Defines, m_Optimize, m_Symbols};
    }

    Project ProjectBuilder::build()
    {
        return Project{m_Name, m_Kind, m_CppVersion, m_Language, m_Architecture, m_TargetDir, m_Location, std::move(m_Builder)};
    }
}