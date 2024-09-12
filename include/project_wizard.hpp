#pragma once

#include <system_error>
#include <filesystem>
#include <iostream>
#include <string>

#define RETURN_IF_ERROR(ec)                                             \
    if (ec)                                                             \
    {                                                                   \
        std::cout << "An error occured: " << ec.message() << std::endl; \
        return;                                                         \
    }

namespace tram
{
    inline void create_project(const std::string &name, bool init_git) noexcept
    {
        namespace fs = std::filesystem;

        std::error_code ec;
        bool exists = fs::exists(name, ec);

        RETURN_IF_ERROR(ec)

        if (exists)
        {
            std::cout << "Project folder does already exists!" << std::endl;
            return;
        }

        fs::create_directory(name, ec);

        RETURN_IF_ERROR(ec)

        // TODO (paul) create basic config file

        if (init_git)
        {
            // TODO (paul) init git repo
        }
    }
}