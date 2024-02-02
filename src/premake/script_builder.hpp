#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <cstdint>

namespace Tram
{
    namespace Premake
    {
        enum class ProjectKind
        {
            APP,

            LIBRARY_DYNAMIC,

            LIBRARY_STATIC,
        };

        enum class CppVersion : uint16_t
        {
            CPP_98 = 98,

            CPP_11 = 11,

            CPP_14 = 14,

            CPP_17 = 17,

            CPP_20 = 20,
        };

        struct ScriptInfo
        {
            std::string workspace_name;

            std::string project_name;

            ProjectKind kind;

            CppVersion cpp_version;
        };

        class ScriptBuilder
        {
        public:
            ScriptBuilder(const ScriptBuilder &info) = delete;

            ScriptBuilder &operator=(const ScriptBuilder &info) = delete;

            static bool build(const ScriptInfo &&info) noexcept
            {
                return get().iBuild(std::move(info));
            };

        private:
            ScriptBuilder() noexcept {};

            static ScriptBuilder &get() noexcept
            {
                static ScriptBuilder instance;

                return instance;
            }

            bool iBuild(const ScriptInfo &&info) noexcept
            {
                std::ofstream file_out{"todo.lua", std::ios::trunc};

                file_out << "workspace \"" << info.workspace_name << "\"\n\tconfigurations {\"Debug\", \"Release\"}\n\n";

                file_out << "project \"" << info.project_name << "\"\n";

                file_out << "\tkind \"" << (info.kind == ProjectKind::APP ? "ConsoleApp" : info.kind == ProjectKind::LIBRARY_DYNAMIC ? "SharedLib"
                                                                                                                                     : "StaticLib")
                         << "\"\n";

                file_out << "\tlanguage \"C++\"\n";
                file_out << "\tcppdialect \"C++" << static_cast<std::underlying_type<CppVersion>::type>(info.cpp_version) << "\"\n";
                file_out << "\tarchitecture \"x86_64\"\n";
                file_out << "\ttargetdir \"bin/%{cfg.buildcfg}\"\n";
                file_out << "\tlocation \"build/\"\n";

                file_out << "\n\n\n";

                // Links

                // Inlcude Dirs

                file_out << "\tfiles {\"src/**.cpp\"" << (info.kind == ProjectKind::APP ? ", \"src/**.hpp\", \"src/**.h\"" : "") << "}\n\n";

                file_out << "\tfilter \"configurations:Debug\"\n\t\tdefines { \"DEBUG \"}\n\t\tsymbols \"On\"\n\n";

                file_out << "\tfilter \"configurations:Release\"\n\t\tdefines { \"NDEBUG \"}\n\t\tsymbols \"On\"\n";

                return file_out.good();
            }
        };
    }
}
