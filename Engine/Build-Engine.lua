project "Engine"
kind "StaticLib"
language "C++"
cppdialect "C++20"
targetdir "Binaries/%{cfg.buildcfg}"
staticruntime "off"
externalwarnings "Off"
externalanglebrackets "On"

pchheader "pch.h"
pchsource "Source/pch.cpp"

files { 
   "Source/**.h", 
   "Source/**.cpp",
   "ThirdParty/*.h"
}

includedirs
{
   "Source",
   "ThirdParty"
}

vsprops {
   VcpkgEnableManifest = "true"
}

targetdir ("../Binaries/" .. OutputDir .. "/%{prj.name}")
objdir ("../Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

filter { "files:ThirdParty/**" }
   flags {"NoPCH"}
   warnings "Off"
filter {}

filter "system:windows"
   systemversion "latest"
   defines { }

filter "configurations:Debug"
   defines { "DEBUG" }
   runtime "Debug"
   symbols "On"

filter "configurations:Release"
   defines { "RELEASE" }
   runtime "Release"
   optimize "On"
   symbols "On"

filter "configurations:Dist"
   defines { "DIST" }
   runtime "Release"
   optimize "On"
   symbols "Off"