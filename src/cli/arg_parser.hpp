#pragma once

#include "../util/strings.hpp"

#include <iostream>
#include <vector>

namespace Tram
{
    namespace CLI
    {
        void handleArgs(const std::vector<std::string> &args)
        {
            if (args.empty())
                return;
        }
    }
}