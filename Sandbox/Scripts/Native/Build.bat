@echo off
setlocal enableDelayedExpansion

set "VANTA_DEBUG=E:\Code\engine\Vanta\build\x64-Debug-MSVC\bin"
set "VANTA_RELEASE=E:\Code\engine\Vanta\build\x64-Release-MSVC\bin"

if "%~1"=="Release" (
	set "BUILD_TYPE=Release"
) else (
	set "BUILD_TYPE=Debug"
)

if "%BUILD_TYPE%"=="Release" (
	set "VANTA_LIB=%VANTA_RELEASE%\Resources\Scripts\Native
	set "VANTA_INCL=%VANTA_RELEASE%\Resources\Scripts\Native\include
) else (
    set "VANTA_LIB=%VANTA_DEBUG%\Resources\Scripts\Native"
	set "VANTA_INCL=%VANTA_DEBUG%\Resources\Scripts\Native\include"
)

cmake -S . -B Build -G "Visual Studio 17 2022" -DVANTA_LIB:STRING=%VANTA_LIB% -DVANTA_INCLUDE:STRING=%VANTA_INCL%
cmake --build Build --config %BUILD_TYPE%

pause