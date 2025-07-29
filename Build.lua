-- premake5.lua

os.execute('del /q *.sln *.vcxproj *.vcxproj.filters 2>nul')

workspace "Sandbox"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "Sandbox"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

group ""
include "Engine/Build-Engine.lua"
include "Sandbox/Build-Sandbox.lua"
include "Squishies/Build-Squishies.lua"