@echo off
echo Generating Visual Studio 2026 solution using Premake...

rem -- generate VS 2026 solution
premake5 vs2026

echo Done!
echo Open Opac.sln
pause