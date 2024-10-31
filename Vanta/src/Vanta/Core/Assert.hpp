#include "Vanta/Util/StaticString.hpp"

#ifndef VANTA_DISABLE_ASSERTS
#   define VANTA_ASSERT(x, ...)      (!(x) ? (VANTA_CRITICAL("Assertion '{}' failed: {} ({}:{})", #x, FMT(__VA_ARGS__), VANTA_FILENAME, VANTA_LINE), VANTA_DEBUGBREAK()) : (void()))
#   define VANTA_CORE_ASSERT(x, ...) (!(x) ? (VANTA_CORE_CRITICAL("Assertion '{}' failed: {} ({}:{})", #x, FMT(__VA_ARGS__), VANTA_FILENAME, VANTA_LINE), VANTA_DEBUGBREAK()) : (void()))
#else
#   define VANTA_ASSERT(x, ...)      (void())
#   define VANTA_CORE_ASSERT(x, ...) (void())
#endif

#define VANTA_UNIMPLEMENTED()      (VANTA_CORE_WARN("{} not implemented ({}:{})", VANTA_FUNCNAME, VANTA_FILENAME, VANTA_LINE))
#define VANTA_UNIMPLEMENTED_FAIL() (VANTA_CORE_CRITICAL("{} not implemented ({}:{})", VANTA_FUNCNAME, VANTA_FILENAME, VANTA_LINE), VANTA_DEBUGBREAK())

#define VANTA_UNREACHABLE(...) (VANTA_CORE_CRITICAL("Entered unreachable code: {} ({}:{})", FMT(__VA_ARGS__), VANTA_FILENAME, VANTA_LINE), VANTA_DEBUGBREAK())
