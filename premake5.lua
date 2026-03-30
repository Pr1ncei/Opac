-- DEPRECATED: This project now uses CMake. This file will be removed in future versions.
-- premake5.lua

local QT_ROOT = "F:/Qt/6.11.0/msvc2022_64"

workspace "Opac"
  architecture "x64"
  startproject "Opac"
  configurations { "Debug", "Release" }

project "Opac"
  kind "ConsoleApp"
  language "C++"
  cppdialect "C++17"

  targetdir ("bin/%{cfg.buildcfg}")
  objdir ("bin-int/%{cfg.buildcfg}")

  defines {
    "STATIC_CONCPP",
    "QT_WIDGETS_LIB",
    "QT_GUI_LIB",
    "QT_CORE_LIB",
    "_CRT_SECURE_NO_WARNINGS"
  }

  files {
    "src/**.cpp",
    "src/**.h",
    "src/**.hpp",

    "moc/**.cpp"
  }

  includedirs {
    "src",
    "moc", 
    "lib/mysql/include/jdbc/",
    QT_ROOT .. "/include",
    QT_ROOT .. "/include/QtCore",
    QT_ROOT .. "/include/QtGui",
    QT_ROOT .. "/include/QtWidgets",
  }

  libdirs {
    "lib/mysql/lib64/vs14/",
    QT_ROOT .. "/lib",
  }

  prebuildcommands {
    '{MKDIR} moc',
    'for %%f in (src\\ui\\*.h) do "' .. QT_ROOT .. '/bin/moc.exe" "%%f" -o "moc\\moc_%%~nf.cpp"'
  }

  filter "configurations:Debug"
    symbols "On"
    runtime "Debug"

    links {
      "mysqlcppconn",
      "Qt6Cored",
      "Qt6Guid",
      "Qt6Widgetsd",
    }

  filter "configurations:Release"
    optimize "On"
    runtime "Release"

    links {
      "mysqlcppconn",
      "Qt6Core",
      "Qt6Gui",
      "Qt6Widgets",
    }

  filter {}