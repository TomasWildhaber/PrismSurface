:: PrismSurface cleanup

@echo off
setlocal

for %%I in ("%~dp0..\..") do set "TARGET_DIR=%%~fI"

for %%D in (bin lib out build PrismSurface\config) do (
    if exist "%TARGET_DIR%\%%D\" (
        rmdir /s /q "%TARGET_DIR%\%%D"
    )
)

echo Cleanup complete!
pause