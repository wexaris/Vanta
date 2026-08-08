cmake_minimum_required(VERSION 3.20)
project(Vanta-ScriptCore-Native LANGUAGES C CXX)

include("${CMAKE_SOURCE_DIR}/cmake/NativeScriptProject.cmake")

set(${PROJECT_NAME}_SOURCE
    "Vanta/Module/Interface.cpp"
    "Vanta/Scene/Components.cpp"
)

vanta_add_native_script_module(${PROJECT_NAME}
    TYPE STATIC
    SOURCES ${${PROJECT_NAME}_SOURCE}
    INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}"
    OUTPUT_DIRECTORY "${VANTA_SCRIPT_DIRECTORY}/Native"
)

# Include Vanta/Base/* for platform definitions and types
add_custom_command(
    TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/Vanta/src/Vanta/Base" "${CMAKE_CURRENT_SOURCE_DIR}/Vanta/Base")

# Use these source files as NativeScript include files
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Vanta" "${VANTA_SCRIPT_DIRECTORY}/Native/include/Vanta")
