workspace "Tram"
    configurations {"Debug", "Release"}

project "Tram"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    architecture "x86_64"
    targetdir "bin/%{cfg.buildcfg}"
    location "build/"

    warnings "Extra"

    links{"curl", "miniz"}

    includedirs{"libs/toml11/include", "libs/ArgParser/include"}

    files {"src/**.cpp"}

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"


    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
        symbols "Off"
        