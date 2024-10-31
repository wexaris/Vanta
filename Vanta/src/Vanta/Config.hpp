#pragma once

//#define VANTA_DISABLE_ASSERTS

//#define VANTA_ENABLE_PROFILE
//#define VANTA_ENABLE_PROFILE_RENDER

#define VANTA_OPENGL_VERSION_MAJOR 4
#define VANTA_OPENGL_VERSION_MINOR 6

#define NOMINMAX                      // Prevent Windows min/max definitions

#define GLM_ENABLE_EXPERIMENTAL       // Allow use of GLM_GTX_transform extensions

#define GLFW_INCLUDE_NONE             // Prevent GLFW from including its own OpenGL

#define IMGUI_IMPL_OPENGL_LOADER_GLAD // Make ImGui use GLAD for OpenGL
