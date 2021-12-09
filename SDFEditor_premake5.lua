workspace "SDFEditor"
    location "./"
    language "C++"
    architecture "x86_64"
    --configurations { "Debug", "Debug Editor", "Development", "Development Editor", "Profile", "Final" }
    configurations { "Debug", "Release" }

    --platforms { "Desktop", "Editor", "iOS", "Android" }

    characterset "ASCII"

    targetdir "./Bin/%{prj.name}/%{cfg.longname}"

    objdir ("./Intermediate/%{prj.name}/%{cfg.longname}")

    filter { "system:windows" }
        defines { "_CRT_SECURE_NO_WARNINGS" }

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
    debugdir "./Bin"
    
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
        "./Source/**.natvis"
    }
    excludes { "./../../Engine/Source/ThirdParty/imgui/misc/**.*" }
    --vpaths { ["Source/**"] = "./../../Engine/Source/**.*" }

    --OpenGL system libraries
    filter { "system:windows" }
        links { "OpenGL32" }

    filter { "system:not windows" }
        links { "GL" }

    