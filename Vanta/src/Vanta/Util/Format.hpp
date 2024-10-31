#pragma once
#include "Vanta/Core/Log.hpp"
#include "Vanta/Input/Input.hpp"

#define FMT(...) (::fmt::format(__VA_ARGS__))

template<> struct fmt::formatter<Vanta::Path> : fmt::ostream_formatter {};
template<> struct fmt::formatter<Vanta::KeyCode> : fmt::ostream_formatter {};
template<> struct fmt::formatter<Vanta::MouseCode> : fmt::ostream_formatter {};
template<> struct fmt::formatter<Vanta::UUID> : ostream_formatter {};
