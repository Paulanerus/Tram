#pragma once

#include <string>
#include <cstdlib>

namespace Tram
{
    namespace Util
    {
        bool ExecuteConsoleCmd(const std::string &cmd)
        {
            if (cmd.empty())
                return false;

            return std::system(cmd.c_str()) == 0;
        }
    }
}