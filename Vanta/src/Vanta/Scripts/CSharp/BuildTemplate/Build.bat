R"=(
@echo off

set "VANTA_DEBUG=E:\Code\engine\Vanta\build\x64-Debug-MSVC\bin"
set "VANTA_RELEASE=E:\Code\engine\Vanta\build\x64-Release-MSVC\bin"

if "%~1"=="Release" (
	set "BUILD_TYPE=Release"
) else (
	set "BUILD_TYPE=Debug"
)

if "%BUILD_TYPE%"=="Release" (
	set "VANTA_LIB=%VANTA_DEBUG%\Resources\Scripts\CSharp\Vanta-ScriptCore-CSharp.dll"
) else (
    set "VANTA_LIB=%VANTA_RELEASE%\Resources\Scripts\CSharp\Vanta-ScriptCore-CSharp.dll"
)

cmake -S . -B Build -DVANTA_LIB:STRING=%VANTA_LIB%
cmake --build Build --config %BUILD_TYPE%

pause
)="