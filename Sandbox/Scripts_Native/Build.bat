@echo off

set "VANTA_INCLUDE_DIR=E:\Code\engine\Vanta\Vanta\src"

if "%~1"=="" (
	set "BUILD_TYPE=Debug"
) else (
	set "BUILD_TYPE=%1"
)

cmake -S . -B Build -G "Visual Studio 17 2022" -DVANTA_INCLUDE_DIR:STRING=%VANTA_INCLUDE_DIR%
cmake --build Build --config %BUILD_TYPE%
