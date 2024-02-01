#pragma once

#include <vector>
#include <string>
#include <algorithm>

namespace Tram
{
    namespace Util
    {
        void ToLower(std::string &str_in)
        {
            std::transform(str_in.begin(), str_in.end(), str_in.begin(), [](unsigned char c)
                           { return std::tolower(c); });
        }

        std::vector<std::string> AsVector(const size_t argc, char *argv[], size_t start = 0)
        {
            if (start >= argc)
                return {};

            std::vector<std::string> strings;
            strings.reserve(argc - start);

            for (size_t i = start; i < argc; i++)
            {
                strings.emplace_back(argv[i]);

                ToLower(strings[i - start]);
            }

            return strings;
        }
    }
}