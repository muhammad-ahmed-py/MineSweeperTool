@echo off
REM --- CONFIGURATION ---
set UE_ENGINE_DIR=E:\UnrealEngine-5.4.4-release
set PLUGIN_DIR=%~dp0\MineSweeper
set OUTPUT_DIR=%~dp0\Packaged

REM --- REMOVE OLD BUILD ---
if exist "%OUTPUT_DIR%" (
    echo Deleting previous build...
    rmdir /s /q "%OUTPUT_DIR%"
)

REM --- PACKAGE THE PLUGIN ---
echo Packaging the plugin...
"%UE_ENGINE_DIR%\Engine\Build\BatchFiles\RunUAT.bat" BuildPlugin ^
    -Plugin="%PLUGIN_DIR%\MineSweeper.uplugin" ^
    -Package="%OUTPUT_DIR%" ^
    -TargetPlatforms=Win64 ^
    -Rocket

REM --- FINISH ---
echo Done! Press any key to exit...
pause