#pragma once

/// ///////////////////// PLATFORM ////////////////////////
/// VANTA_PLATFORM_WINDOWS
/// VANTA_PLATFORM_IOS
/// VANTA_PLATFORM_MACOS
/// VANTA_PLATFORM_ANDROID
/// VANTA_PLATFORM_LINUX

#if defined(_WIN32) || defined(_WIN64)
    #define VANTA_PLATFORM_WINDOWS
#elif defined(__APPLE__) || defined(__MACH__)
    #include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR == 1
        #error "IOS simulator is not supported!"
    #elif TARGET_OS_IPHONE == 1
        #define VANTA_PLATFORM_IOS
        #error "IOS is not supported!"
    #elif TARGET_OS_MAC == 1
        #define VANTA_PLATFORM_MACOS
        #error "MacOS is not supported!"
    #else
        #error "Unknown platform!"
    #endif
#elif defined(__ANDROID__)
    #define VANTA_PLATFORM_ANDROID
    #error "Android is not supported!"
#elif defined(__linux__)
    #define VANTA_PLATFORM_LINUX
    #error "Linux is not supported!"
#else
    #error Unknown platform!
#endif


/// 
/// 
/// VANTA_EXPORT - set externally

#if defined(VANTA_PLATFORM_WINDOWS)
    #if defined(VANTA_MODULE)
        #define VANTA_EXPORT extern "C" __declspec(dllexport)
    #else
        #define VANTA_EXPORT extern "C" __declspec(dllimport)
    #endif
#elif defined(VANTA_PLATFORM_LINUX)
    #define VANTA_EXPORT
#endif


/// ///////////////////// BUILD TYPE //////////////////////
/// VANTA_DEBUG
/// VANTA_RELEASE
/// VANTA_DISTRIB - set externally

#ifndef VANTA_DISTRIB
    #ifndef NDEBUG
        #define VANTA_DEBUG
    #else
        #define VANTA_RELEASE
    #endif
#endif


/// ///////////////////// SETTINGS ////////////////////////
/// VANTA_ENABLE_PROFILE
/// VANTA_ENABLE_PROFILE_RENDER
///
/// VANTA_DISABLE_ASSERTS

#ifdef VANTA_DEBUG
    #define VANTA_ENABLE_PROFILE
    #define VANTA_ENABLE_PROFILE_RENDER
#else
    #define VANTA_DISABLE_ASSERTS
#endif


#define NOMINMAX                      // Prevent Windows min/max definitions

#define GLM_ENABLE_EXPERIMENTAL       // Allow use of GLM_GTX_transform extensions

#define GLFW_INCLUDE_NONE             // Prevent GLFW from including its own OpenGL

#define IMGUI_IMPL_OPENGL_LOADER_GLAD // Make ImGui use GLAD for OpenGL


/// //////////////////// META /////////////////////////////
/// VANTA_FILEPATH
/// VANTA_FILENAME
/// VANTA_LINE
/// VANTA_FUNCSIG_RAW
/// VANTA_FUNCSIG
/// VANTA_FUNCNAME
/// VANTA_DEBUGBREAK()
/// 
/// VANTA_ASSERT(x, ...)
/// VANTA_CORE_ASSERT(x, ...)

#define VANTA_FILEPATH __FILE__
#define VANTA_FILENAME (::Vanta::StaticUtil::FileName(VANTA_FILEPATH))
#define VANTA_LINE __LINE__

#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
    #define VANTA_FUNCSIG_RAW __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
    #define VANTA_FUNCSIG_RAW __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__) || defined(_MSC_VER)
    #define VANTA_FUNCSIG_RAW __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
    #define VANTA_FUNCSIG_RAW __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
    #define VANTA_FUNCSIG_RAW __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
    #define VANTA_FUNCSIG_RAW __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
    #define VANTA_FUNCSIG_RAW __func__
#else
    #define VANTA_FUNCSIG_RAW "UNKNOWN"
#endif

/// Function signature (with parameter list and return type)
#define VANTA_FUNCSIG (::Vanta::StaticUtil::SignatureClean(VANTA_FUNCSIG_RAW).Data)
/// Function signature (without parameter list and return type)
#define VANTA_FUNCNAME (::Vanta::StaticUtil::SignatureName(VANTA_FUNCSIG).Data)

#if defined(VANTA_PLATFORM_WINDOWS)
    #define VANTA_DEBUGBREAK() __debugbreak()
#elif defined(VANTA_PLATFORM_LINUX)
    #include <signal.h>
    #define VANTA_DEBUGBREAK() raise(SIGTRAP)
#else
    #define VANTA_DEBUGBREAK()
    #error "Platform missing debugbreak!"
#endif


/// ///////////////////// DEBUG ///////////////////////////
/// CONCAT(a, b)
/// 
/// VANTA_ASSERT(x, ...)
/// VANTA_CORE_ASSERT(x, ...)
/// 
/// VANTA_UNIMPLEMENTED()
/// VANTA_UNIMPLEMENTED_FAIL()
/// 
/// VANTA_UNREACHABLE(...)

#define CONCAT_(a, b) a ## b
#define CONCAT(a, b) CONCAT_(a, b)

#ifndef VANTA_DISABLE_ASSERTS
    #define VANTA_ASSERT(x, ...)      (!(x) ? (VANTA_CRITICAL("Assertion '{}' failed: {} ({}:{})", #x, FMT(__VA_ARGS__), VANTA_FILENAME, VANTA_LINE), VANTA_DEBUGBREAK()) : (void()))
    #define VANTA_CORE_ASSERT(x, ...) (!(x) ? (VANTA_CORE_CRITICAL("Assertion '{}' failed: {} ({}:{})", #x, FMT(__VA_ARGS__), VANTA_FILENAME, VANTA_LINE), VANTA_DEBUGBREAK()) : (void()))
#else
    #define VANTA_ASSERT(x, ...)      (void())
    #define VANTA_CORE_ASSERT(x, ...) (void())
#endif

#define VANTA_UNIMPLEMENTED()      (VANTA_CORE_WARN("{} not implemented ({}:{})", VANTA_FUNCNAME, VANTA_FILENAME, VANTA_LINE))
#define VANTA_UNIMPLEMENTED_FAIL() (VANTA_CORE_CRITICAL("{} not implemented ({}:{})", VANTA_FUNCNAME, VANTA_FILENAME, VANTA_LINE), VANTA_DEBUGBREAK())

#define VANTA_UNREACHABLE(...) (VANTA_CORE_CRITICAL("Entered unreachable code: {} ({}:{})", FMT(__VA_ARGS__), VANTA_FILENAME, VANTA_LINE), VANTA_DEBUGBREAK())


/// ///////////////////// MISC ////////////////////////////
/// BIT(x)
/// STRING(x)
/// UNUSED(...)

#define BIT(x) (1 << x)
#define STRING(x) #x

#define UNUSED(...) (void)(__VA_ARGS__)
