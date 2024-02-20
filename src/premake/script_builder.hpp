#pragma once

#include "../config/config.hpp"

#include <string>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <algorithm>

namespace Tram
{
    class ScriptBuilder
    {
    public:
        ScriptBuilder(const ScriptBuilder &builder) = delete;

        ScriptBuilder &operator=(const ScriptBuilder &builder) = delete;

        static bool build(const TramConfig &config) noexcept
        {
            return get().iBuild(config);
        };

    private:
        ScriptBuilder() noexcept {};

        static ScriptBuilder &get() noexcept
        {
            static ScriptBuilder instance;

            return instance;
        }

        bool iBuild(const TramConfig &config) noexcept
        {
            std::ofstream file_out{"tram.premake5.lua", std::ios::trunc};

            if (!file_out.is_open())
                return false;

            const auto info = config.info();

            file_out << "workspace \"" << info.project_name << "\"\n\tconfigurations {\"Debug\", \"Release\"}\n\n";

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
