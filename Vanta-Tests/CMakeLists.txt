cmake_minimum_required(VERSION 3.20)
project(Testing)

set(${PROJECT_NAME}_SOURCE
    "src/Tests.cpp"
)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCE})

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(${PROJECT_NAME} PRIVATE Vanta)

target_compile_options(${PROJECT_NAME} PRIVATE
    #DEBUG
    $<$<CONFIG:DEBUG>:
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
            -Wall -Wextra -pedantic -O0 -g -Wno-unused-function -Wno-format-security -Wno-language-extension-token>
        $<$<CXX_COMPILER_ID:MSVC>:
            /W3 /ZI /RTC1 /sdl /Od>>
)
