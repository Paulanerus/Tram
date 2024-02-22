#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <iterator>

namespace Tram
{
    namespace Util
    {
        void ToLower(std::string &str_in)
        {
            std::transform(str_in.begin(), str_in.end(), str_in.begin(), [](unsigned char c)
                           { return std::tolower(c); });
        }

        std::string Join(const std::vector<std::string> &vec, const char *delimiter = ", ")
        {
            std::ostringstream stream;

            auto begin = vec.begin(), end = vec.end();

            if (begin != end)
            {
                std::copy(begin, std::prev(end), std::ostream_iterator<std::string>(stream, delimiter));
                begin = prev(end);
            }

            if (begin != end)
                stream << *begin;

            return stream.str();
        }
    }
}