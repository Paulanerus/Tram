#include <iostream>

#include <arg_parser.hpp>
#include <toml.hpp>

int main(int argc, char *argv[])
{
    toml::color::enable();

    psap::ArgParser parser{"tram"};

    parser.parse(argv, argc);
}