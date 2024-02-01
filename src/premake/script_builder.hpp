#pragma once

#include <string>

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

        struct ScriptInfo
        {
            std::string workspace_name;

            std::string project_name;

            ProjectKind kind;

            size_t cpp_version;
        };

        class ScriptBuilder
        {
        public:
        private:
        };
    }
}
