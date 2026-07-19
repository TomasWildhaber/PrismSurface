:: Visual Studio 2022 PrismSurface setup

@echo off
echo Setting up PrismSurface for Visual Studio 2026...
echo Make sure you are using CMake 4.2 or higher

set "IDE_GENERATOR=Visual Studio 18 2026"
call Setup.bat %*
