cmake_minimum_required(VERSION 3.20)
project(Sandbox)

set(${PROJECT_NAME}_SOURCE
    "src/Sandbox.cpp"
)

if(WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${${PROJECT_NAME}_SOURCE})
else()
    add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCE})
endif()

target_include_directories(${PROJECT_NAME} PUBLIC src)

target_link_libraries(${PROJECT_NAME} PUBLIC Vanta)

target_compile_options(${PROJECT_NAME} PUBLIC
    #DEBUG
    $<$<CONFIG:DEBUG>:
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
            -Wall -Wextra -O0 -g -Wno-unused-function>
        $<$<CXX_COMPILER_ID:MSVC>:
            /W3 /ZI /RTC1 /sdl /Od>>
    #RELEASE
    $<$<CONFIG:RELEASE>:
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
            -O2 -s>
        $<$<CXX_COMPILER_ID:MSVC>:
            /Zi /GL /O2>>
)

# Remove /W3 from MSVC flags
string(REGEX REPLACE "/W3" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
