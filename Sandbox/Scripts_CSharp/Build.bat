@echo off

set "BUILD_TYPE=Debug"

set "CORE_LIB_DEBUG=E:\Code\engine\Vanta\build\x64-Debug-MSVC\lib\Vanta-ScriptCore-CSharp.dll"
set "CORE_LIB_RELEASE=E:\Code\engine\Vanta\build\x64-Debug-MSVC\lib\Vanta-ScriptCore-CSharp.dll"

if "%BUILD_TYPE%"=="Debug" (
    set "CORE_LIB=%CORE_LIB_DEBUG%"
) else (
    set "CORE_LIB=%CORE_LIB_RELEASE%"
)

cmake -S . -B Build -DVANTA_CORE_LIB:STRING=%CORE_LIB%
cmake --build Build --config %BUILD_TYPE%

pause
