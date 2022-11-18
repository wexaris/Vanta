cmake_minimum_required(VERSION 3.20)
project(Editor)

set(${PROJECT_NAME}_SOURCE
    "src/Editor/Editor.cpp"
    "src/Editor/EditorLayer.cpp"
    "src/Editor/CameraController.cpp"
    "src/Editor/Window/Console.cpp"
    "src/Editor/Window/ContentBrowser.cpp"
    "src/Editor/Window/SceneHierarchy.cpp"
)

if(WIN32)
    add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCE})
else()
    add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCE})
endif()

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(${PROJECT_NAME} PRIVATE Vanta)

target_compile_options(${PROJECT_NAME} PRIVATE
    #DEBUG
    $<$<CONFIG:DEBUG>:
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
            -Wall -Wextra -pedantic -O0 -g -Wno-unused-function -Wno-format-security -Wno-language-extension-token>
        $<$<CXX_COMPILER_ID:MSVC>:
            /W3 /ZI /RTC1 /sdl /Od>>
    #RELEASE
    $<$<CONFIG:RELEASE>:
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
            -Wall -Wextra -pedantic -O2 -Wno-unused-function -Wno-format-security -Wno-language-extension-token>
        $<$<CXX_COMPILER_ID:MSVC>:
            /Zi /GL /O2>>
)

# Copy assets to binary directory
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/resources" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Resources")
