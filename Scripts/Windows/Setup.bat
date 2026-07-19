:: PrismSurface setup

@echo off

:: Check if an argument was provided
if "%IDE_GENERATOR%"=="" (
    echo Error: Provide a CMake generator argument.
    pause
)

cd "../../"
set "FOLDER_NAME=build"

if not exist "%FOLDER_NAME%" (
    mkdir "%FOLDER_NAME%"
)

cd "%FOLDER_NAME%"

cmake -G "%IDE_GENERATOR%" %* ..

pause