cmake_minimum_required(VERSION 3.20)
project(VantaTests)

include("${CMAKE_SOURCE_DIR}/cmake/NativeScriptProject.cmake")

set(CMAKE_CXX_STANDARD 20)

set(VANTA_TEST_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

function(vanta_configure_test_target target)
    target_include_directories(${target} PRIVATE "${VANTA_TEST_SOURCE_DIR}")
    target_link_libraries(${target} PRIVATE VantaTestUtils)

    target_compile_options(${target} PRIVATE
        #DEBUG
        $<$<CONFIG:DEBUG>:
            $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
                -Wall -Wextra -pedantic -O0 -g -Wno-unused-function -Wno-format-security -Wno-language-extension-token>
            $<$<CXX_COMPILER_ID:MSVC>:
                /W3 /RTC1 /sdl /Od>>
    )
endfunction()
add_subdirectory(src/vanta-test-utils)
add_subdirectory(src/vanta-core)
add_subdirectory(src/vanta-scripts-native)
