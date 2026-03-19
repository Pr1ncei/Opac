@echo off

echo Creating Visual Studio solution...

REM Create solution
dotnet new sln -n Opac

REM Add existing project
dotnet sln Opac.sln add DIPROGLANG_OPAC.vcxproj

echo Done! Open Opac.sln in Visual Studio.
pause