#pragma once
#include "Vanta/Base/Platform.hpp"

///
///
/// //////////// DLL IMPORT / EXPORT //////////////////////

#if defined(VANTA_PLATFORM_WINDOWS)
#   if defined(VANTA_MODULE)
#       define VANTA_EXPORT extern "C" __declspec(dllexport)
#   else
#       define VANTA_EXPORT extern "C" __declspec(dllimport)
#   endif
#elif defined(VANTA_PLATFORM_LINUX)
#   define VANTA_EXPORT
#endif

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
#define VANTA_FILENAME (::Vanta::StaticString::PathFileName(VANTA_FILEPATH))
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
#define VANTA_FUNCSIG (::Vanta::StaticString::SignatureClean(VANTA_FUNCSIG_RAW).Data)
/// Function signature (without parameter list and return type)
#define VANTA_FUNCNAME (::Vanta::StaticString::SignatureName(VANTA_FUNCSIG).Data)

#if defined(VANTA_PLATFORM_WINDOWS)
    #define VANTA_DEBUGBREAK() __debugbreak()
#elif defined(VANTA_PLATFORM_LINUX)
    #include <signal.h>
    #define VANTA_DEBUGBREAK() raise(SIGTRAP)
#else
    #define VANTA_DEBUGBREAK()
    #error "Platform missing debugbreak!"
#endif

/// ///////////////////// MISC ////////////////////////////
/// CONCAT
/// BIT(x)
/// STRING(x)
/// UNUSED(...)

#define CONCAT_(a, b) a ## b
#define CONCAT(a, b) CONCAT_(a, b)

#define BIT(x) (1 << x)
#define STRING(x) #x

#define UNUSED(...) (void)(__VA_ARGS__)
