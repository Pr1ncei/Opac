-- premake5.lua
workspace "Opac"
  architecture "x64"
  startproject "Opac"

  configurations { "Debug", "Release" }

project "Opac"
  kind "ConsoleApp"
  language "C++"
  cppdialect "C++17"

  -- Source files
  files { 
    "src/**.cpp", 
    "src/**.h", 
    "src/**.hpp" 
  }

  -- Include Directories
  includedirs {
      "src",
      "lib/mysql/include/jdbc/"
  }

  defines { "STATIC_CONCPP" }

  -- Libraries
  libdirs { "lib/mysql/lib64/vs14/" }
  links { "mysqlcppconn" }

  postbuildcommands {
      '{COPY} lib/mysql/lib64/mysqlcppconn-10-vs14.dll bin/Release',
      '{COPY} lib/mysql/lib64/libssl-*.dll bin/Release',
      '{COPY} lib/mysql/lib64/libcrypto-*.dll bin/Release'
  }

  -- Build configurations
  filter "configurations:Debug"
      symbols "On"
      staticruntime "off"
      runtime "Debug"

  filter "configurations:Release"
      optimize "On"
      staticruntime "off"
      runtime "Release"