#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace Tram
{
    enum class ProjectKind : uint8_t
    {
        APP = 0,

        LIBRARY_DYNAMIC = 1,

        LIBRARY_STATIC = 2,
    };

    enum class CppVersion : uint16_t
    {
        CPP_98 = 98,

        CPP_11 = 11,

        CPP_14 = 14,

        CPP_17 = 17,

        CPP_20 = 20,
    };

    const char *VersionToString(const CppVersion &cpp_version)
    {
        switch (cpp_version)
        {
        case CppVersion::CPP_11:
            return "C++11";
        case CppVersion::CPP_14:
            return "C++14";
        case CppVersion::CPP_17:
            return "C++17";
        case CppVersion::CPP_20:
            return "C++20";
        default:
            return "C++98";
        }
    }

    enum class Language : uint8_t
    {
        C = 0,

        CPP = 1,
    };

    enum class Architecture : uint8_t
    {
        X86 = 0,

        X86_64 = 1,

        ARM = 2,

        ARM_64 = 3,
    };

    const char *ArchToString(const Architecture &architecture)
    {
        switch (architecture)
        {
        case Architecture::X86_64:
            return "x86_64";
        case Architecture::ARM:
            return "ARM";
        case Architecture::ARM_64:
            return "ARM64";
        default:
            return "x86";
        }
    }

    enum class Configuration : uint8_t
    {
        CONF_DEBUG = 0,

        CONF_RELEASE = 1,
    };
}