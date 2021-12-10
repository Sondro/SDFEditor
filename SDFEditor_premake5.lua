workspace "SDFEditor"
    location "./"
    language "C++"
    architecture "x86_64"
    
    configurations { "Debug", "Release" }

    characterset "ASCII"

    targetdir "./Bin/%{prj.name}/%{cfg.longname}"

    objdir ("./Intermediate/%{prj.name}/%{cfg.longname}")

    flags { "FatalWarnings" }

    filter { "system:windows" }
        defines { "_CRT_SECURE_NO_WARNINGS" }
        --linkoptions { "/NODEFAULTLIB:msvcrt" }
        ignoredefaultlibraries { "MSVCRT" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
        optimize "Off"

    filter "configurations:not Debug"
        defines { "NDEBUG" }
        symbols "Off"
        optimize "On"  

    filter { }

project "SDFEditor"
    location "./Build"
    kind "ConsoleApp"
    links { "glfw3" }
    targetname "SDFEditor"
    debugdir "./Data"
    
    includedirs { 
        "./Source/", 
        --"./Source/ThirdParty/", 
        "./External/glfw/include"
    }

    libdirs { 
        "./External/glfw/lib-vc2010-64"
    }

    files  {
        "./Source/**.h",
        "./Source/**.c",
        "./Source/**.cpp",
        "./Source/**.def",
        "./Source/**.inl",
        "./Source/**.natvis",
        "./Data/Shaders/*.glsl"
    }

    vpaths { 
        ["Source/**"] = "./Source/**.*",
        ["Shaders/**"] = "./Data/Shaders/**.*"
    }

    excludes { "./Source/ThirdParty/imgui/misc/**.*" }

    --OpenGL system libraries
    filter { "system:windows" }
        links { "OpenGL32" }

    filter { "system:not windows" }
        links { "GL" }

    