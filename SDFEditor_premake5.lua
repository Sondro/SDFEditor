workspace "SDFEditor"
    location "./"
    language "C++"
    architecture "x86_64"
    
    configurations { "Debug", "Release" }

    characterset "ASCII"

    targetdir "./Bin/%{prj.name}/%{cfg.longname}"

    objdir ("./Intermediate/%{prj.name}/%{cfg.longname}")

    flags { "FatalWarnings" }

    defines { "IMGUI_IMPL_OPENGL_LOADER_CUSTOM" }

    cppdialect "c++17"

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

project "sbx"
    location "./Build"
    kind "StaticLib"
    targetname "sbx"
    
    includedirs { 
        "./Source", 
        "./Source/ThirdParty", 
        "./External/glfw/include"
    }

    files  {
        "./Source/sbx/**.h",
        "./Source/sbx/**.hpp",
        "./Source/sbx/**.c",
        "./Source/sbx/**.cpp",
        "./Source/sbx/**.def",
        "./Source/sbx/**.inl",
        "./Source/sbx/**.natvis",
        "./Source/ThirdParty/**.h",
        "./Source/ThirdParty/**.hpp",
        "./Source/ThirdParty/**.c",
        "./Source/ThirdParty/**.cpp",
        "./Source/ThirdParty/**.def",
        "./Source/ThirdParty/**.inl",
        "./Source/ThirdParty/**.natvis",
    }

    vpaths { 
        ["Source/sbx/**"] = "./Source/sbx/**.*",
    }

    excludes { "./Source/ThirdParty/imgui/misc/**.*" }
    excludes { "./Source/ThirdParty/glm/**.cpp" }

project "SDFEditor"
    location "./Build"
    kind "ConsoleApp"
    links { "glfw3", "sbx" }
    targetname "SDFEditor"
    debugdir "./Data"
    
    includedirs { 
        "./Source", 
        "./Source/ThirdParty", 
        "./External/glfw/include"
    }

    libdirs { 
        "./External/glfw/lib-vc2010-64",
        "./Bin/sbx/%{cfg.longname}"
    }

    files  {
        "./Source/SDFEditor/**.h",
        "./Source/SDFEditor/**.hpp",
        "./Source/SDFEditor/**.c",
        "./Source/SDFEditor/**.cpp",
        "./Source/SDFEditor/**.def",
        "./Source/SDFEditor/**.inl",
        "./Source/SDFEditor/**.natvis",
        "./Data/Shaders/*.glsl"
    }

    vpaths { 
        ["Source/SDFEditor/**"] = "./Source/SDFEditor/**.*",
        ["Shaders/**"] = "./Data/Shaders/**.*"
    }

    --excludes { "./Source/ThirdParty/imgui/misc/**.*" }
    --excludes { "./Source/ThirdParty/glm/**.cpp" }

    --OpenGL system libraries
    filter { "system:windows" }
        links { "OpenGL32" }

    filter { "system:not windows" }
        links { "GL" }

    