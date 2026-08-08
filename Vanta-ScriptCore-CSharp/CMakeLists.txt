cmake_minimum_required(VERSION 3.20)
project(Vanta-ScriptCore-CSharp)

# Find the dotnet executable
find_program(DOTNET_EXE dotnet REQUIRED)

# Define source and output locations
set(CS_PROJ_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.csproj")
set(CS_OUT_DIR "${VANTA_SCRIPT_DIRECTORY}/CSharp")

# Map CMake build types to standard .NET configurations
if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR NOT CMAKE_BUILD_TYPE)
    set(DOTNET_CONFIG "Debug")
else()
    set(DOTNET_CONFIG "Release")
endif()

# Ninja target that triggers the cross-platform compile
add_custom_target(${PROJECT_NAME} ALL
    COMMAND ${DOTNET_EXE} build "${CS_PROJ_PATH}"
            --configuration ${DOTNET_CONFIG}
            --output "${CS_OUT_DIR}"
    COMMENT "Compiling C# project '${PROJECT_NAME}' via .NET CLI..."
    VERBATIM
)
