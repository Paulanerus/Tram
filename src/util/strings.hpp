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
    }
}