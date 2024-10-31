
/// ///////////////////// PLATFORM ////////////////////////
/// VANTA_PLATFORM_WINDOWS
/// VANTA_PLATFORM_IOS
/// VANTA_PLATFORM_MACOS
/// VANTA_PLATFORM_ANDROID
/// VANTA_PLATFORM_LINUX

#if defined(_WIN32) || defined(_WIN64)
#   define VANTA_PLATFORM_WINDOWS
#elif defined(__APPLE__) || defined(__MACH__)
#   include <TargetConditionals.h>
#   if TARGET_IPHONE_SIMULATOR == 1
#       error "IOS simulator is not supported!"
#   elif TARGET_OS_IPHONE == 1
#       define VANTA_PLATFORM_IOS
#       error "IOS is not supported!"
#   elif TARGET_OS_MAC == 1
#       define VANTA_PLATFORM_MACOS
#       error "MacOS is not supported!"
#   else
#       error "Unknown platform!"
#   endif
#elif defined(__ANDROID__)
#   define VANTA_PLATFORM_ANDROID
#   error "Android is not supported!"
#elif defined(__linux__)
#   define VANTA_PLATFORM_LINUX
#   error "Linux is not supported!"
#else
#   error Unknown platform!
#endif


/// ///////////////////// BUILD TYPE //////////////////////
/// VANTA_DEBUG
/// VANTA_RELEASE
/// VANTA_DISTRIB - set externally

#ifndef VANTA_DISTRIB
#   ifndef NDEBUG
#       define VANTA_DEBUG
#   else
#       define VANTA_RELEASE
#   endif
#endif
