@echo off

set "CORE_LIB_DEBUG=E:\Code\engine\Vanta\build\x64-Debug-MSVC\lib\Vanta-ScriptCore-CSharp.dll"
set "CORE_LIB_RELEASE=E:\Code\engine\Vanta\build\x64-Debug-MSVC\lib\Vanta-ScriptCore-CSharp.dll"

if "%~1"=="" (
	set "BUILD_TYPE=Debug"
) else (
	set "BUILD_TYPE=%1"
)

if "%BUILD_TYPE%"=="Release" (
	set "CORE_LIB=%CORE_LIB_RELEASE%"
) else (
    set "CORE_LIB=%CORE_LIB_DEBUG%"
)

cmake -S . -B Build -DVANTA_CORE_LIB:STRING=%CORE_LIB%
cmake --build Build --config %BUILD_TYPE%
