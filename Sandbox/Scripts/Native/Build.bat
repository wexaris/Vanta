@echo off
setlocal enableDelayedExpansion

set "VANTA_DEBUG=E:\Engine\Vanta\out\build\x64-debug\bin"
set "VANTA_RELEASE=E:\Engine\Vanta\out\build\x64-release\bin"

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

cmake -S . -B Build -DVANTA_LIB:STRING=%VANTA_LIB% -DVANTA_INCLUDE:STRING=%VANTA_INCL%
cmake --build Build --config %BUILD_TYPE%

pause