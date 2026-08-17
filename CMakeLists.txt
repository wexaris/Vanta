cmake_minimum_required(VERSION 3.20)
project(VantaProj)

# Set CMake to export compile commands for tools like clangd and ccls
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
)

if(POLICY CMP0141)
	cmake_policy(SET CMP0141 NEW)
	# Use one consistent MSVC debug-info mode to avoid /Zi vs /ZI overrides.
	set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>")
endif()

enable_testing()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_CXX_STANDARD 20)

set(VANTA_SCRIPT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Scripts" CACHE INTERNAL "")

add_subdirectory(Vanta)
add_subdirectory(Vanta-ScriptCore-CSharp)
add_subdirectory(Vanta-ScriptCore-Native)
add_subdirectory(Vanta-Tests)
add_subdirectory(Editor)
