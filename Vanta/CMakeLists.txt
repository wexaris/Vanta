cmake_minimum_required(VERSION 3.20)
project(Vanta LANGUAGES C CXX)

add_subdirectory(vendor)

find_package(Boost REQUIRED COMPONENTS fiber thread)
find_package(box2d CONFIG REQUIRED)
find_package(entt CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(imguizmo CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(spirv_cross_core CONFIG REQUIRED)
find_package(spirv_cross_glsl CONFIG REQUIRED)
find_package(Vulkan REQUIRED COMPONENTS glslang shaderc_combined SPIRV-Tools)
find_package(yaml-cpp CONFIG REQUIRED)
find_path(STB_INCLUDE_DIRS "stb_image.h")

set(${PROJECT_NAME}_SOURCE
    "src/Vanta/Core/Engine.cpp"
    "src/Vanta/Core/EntryPoint.cpp"
    "src/Vanta/Core/Fibers.cpp"
    "src/Vanta/Core/GUILayer.cpp"
    "src/Vanta/Core/LayerStack.cpp"
    "src/Vanta/Core/Log.cpp"
    "src/Vanta/Core/UUID.cpp"
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
    "src/Vanta/Render/UniformBuffer.cpp"
    "src/Vanta/Render/VertexArray.cpp"
    "src/Vanta/Scene/SceneCamera.cpp"
    "src/Vanta/Scene/Serializer.cpp"
    "src/Vanta/Scene/Entity.cpp"
    "src/Vanta/Scene/Scene.cpp"
    "src/Vanta/Script/Interface.cpp"
    "src/Vanta/Script/ScriptEngine.cpp"
    "src/Vanta/Util/Math.cpp"
    "src/Vanta/Util/Time.cpp"
    "src/Platform/OpenGL/Buffer.cpp"
    "src/Platform/OpenGL/Context.cpp"
    "src/Platform/OpenGL/Framebuffer.cpp"
    "src/Platform/OpenGL/GraphicsAPI.cpp"
    "src/Platform/OpenGL/Shader.cpp"
    "src/Platform/OpenGL/Texture.cpp"
    "src/Platform/OpenGL/UniformBuffer.cpp"
    "src/Platform/OpenGL/VertexArray.cpp"
    "src/Platform/Windows/PlatformUtils.cpp"
    "src/Platform/Windows/Window.cpp"
    "src/Platform/Windows/Input.cpp"
 "src/Vanta/Scene/BufferedRegistry.cpp")

set(${${PROJECT_NAME}_PCH} "${CMAKE_CURRENT_SOURCE_DIR}/src/vantapch.hpp")

add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCE})
target_precompile_headers(${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_PCH})

add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}-Script)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(${PROJECT_NAME}
    PUBLIC Boost::boost Boost::fiber Boost::thread EnTT::EnTT fmt::fmt glm::glm
        imgui::imgui imguizmo::imguizmo spdlog::spdlog 
    PRIVATE box2d::box2d glad::glad glfw mono spirv-cross-core spirv-cross-glsl
        Vulkan::Vulkan Vulkan::glslang Vulkan::shaderc_combined Vulkan::SPIRV-Tools yaml-cpp)

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
            /MP /W4 /ZI /RTC1 /sdl /Od>>
    #RELEASE
    $<$<CONFIG:RELEASE>:
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
            -Wall -Wextra -pedantic -O3 -Wno-format-security -Wno-language-extension-token>
        $<$<CXX_COMPILER_ID:MSVC>:
            /MP /Zi /GL /O2>>
)

target_link_options(${PROJECT_NAME} PUBLIC
    $<$<CONFIG:DEBUG>:$<$<CXX_COMPILER_ID:MSVC>:
        /ignore:4099>>
    $<$<CONFIG:RELEASE>:$<$<CXX_COMPILER_ID:MSVC>:
        /LTCG /ignore:4099>>
)

# Copy assets to runtime directory
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/resources" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Resources"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/mono" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mono"
)
