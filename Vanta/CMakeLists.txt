cmake_minimum_required(VERSION 3.20)
project(vanta LANGUAGES C CXX)

#enable_language(ASM_NASM)
#set(NASM_FLAGS "-f win64")
#set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> ${NASM_FLAGS} -o <OBJECT> <SOURCE>")
#set(CMAKE_ASM_NASM_CREATE_STATIC_LIBRARY "<CMAKE_AR> /OUT:<TARGET> <LINK_FLAGS> <OBJECTS>")

find_package(Boost REQUIRED COMPONENTS fiber thread)
find_package(entt CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(imguizmo CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_path(STB_INCLUDE_DIRS "stb_image.h")

set(${PROJECT_NAME}_SOURCE
    "src/Vanta/Core/Engine.cpp"
    "src/Vanta/Core/EntryPoint.cpp"
    "src/Vanta/Core/Fibers.cpp"
    "src/Vanta/Core/GUILayer.cpp"
    "src/Vanta/Core/LayerStack.cpp"
    "src/Vanta/Core/Log.cpp"
    "src/Vanta/Core/Window.cpp"
    "src/Vanta/Debug/Instrumentor.cpp"
    "src/Vanta/Input/Input.cpp"
    "src/Vanta/IO/File.cpp"
    "src/Vanta/IO/Image.cpp"
    "src/Vanta/Render/Buffer.cpp"
    "src/Vanta/Render/Camera.cpp"
    "src/Vanta/Render/Framebuffer.cpp"
    "src/Vanta/Render/GraphicsAPI.cpp"
    "src/Vanta/Render/GraphicsContext.cpp"
    "src/Vanta/Render/RenderCommand.cpp"
    "src/Vanta/Render/Renderer.cpp"
    "src/Vanta/Render/Renderer2D.cpp"
    "src/Vanta/Render/Shader.cpp"
    "src/Vanta/Render/Texture.cpp"
    "src/Vanta/Render/VertexArray.cpp"
    "src/Vanta/Scene/Components.cpp"
    "src/Vanta/Scene/SceneCamera.cpp"
    "src/Vanta/Scene/Serializer.cpp"
    "src/Vanta/Scene/Entity.cpp"
    "src/Vanta/Scene/Scene.cpp"
    "src/Vanta/Util/Math.cpp"
    "src/Vanta/Util/Util.cpp"
    "src/Platform/OpenGL/Render/Buffer.cpp"
    "src/Platform/OpenGL/Render/Context.cpp"
    "src/Platform/OpenGL/Render/Framebuffer.cpp"
    "src/Platform/OpenGL/Render/GraphicsAPI.cpp"
    "src/Platform/OpenGL/Render/Shader.cpp"
    "src/Platform/OpenGL/Render/Texture.cpp"
    "src/Platform/OpenGL/Render/VertexArray.cpp"
    "src/Platform/Windows/PlatformUtils.cpp"
    "src/Platform/Windows/Core/Window.cpp"
    "src/Platform/Windows/Input/Input.cpp"
)

set(${${PROJECT_NAME}_PCH} src/vantapch.hpp)

add_library(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCE})
target_precompile_headers(${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_PCH})

target_include_directories(${PROJECT_NAME} PUBLIC src)

target_link_libraries(${PROJECT_NAME}
    PUBLIC EnTT::EnTT fmt::fmt glm::glm spdlog::spdlog
    PRIVATE Boost::boost Boost::fiber Boost::thread glad::glad glfw imgui::imgui imguizmo::imguizmo yaml-cpp)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<BOOL:${VANTA_DISTRIB}>:
        -DVANTA_DISTRIB>
)

target_compile_options(${PROJECT_NAME} PRIVATE
    #DEBUG
    $<$<CONFIG:DEBUG>:
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
            -Wall -Wextra -pedantic -O0 -g -Wno-unused-function -Wno-format-security -Wno-language-extension-token>
        $<$<CXX_COMPILER_ID:MSVC>:
            /MDd /W4 /ZI /RTC1 /sdl /Od>>
    #RELEASE
    $<$<CONFIG:RELEASE>:
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
            -O2 -s>
        $<$<CXX_COMPILER_ID:MSVC>:
            /MD /Zi /GL /O2>>
)

target_link_options(${PROJECT_NAME} PUBLIC
    $<$<CONFIG:RELEASE>:$<$<CXX_COMPILER_ID:MSVC>:
        /SUBSYSTEM:WINDOWS /LTCG>>
)

# Remove /W3 from MSVC flags
#string(REGEX REPLACE "/W3" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets)
