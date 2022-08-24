#pragma once
#include <Vanta/Vanta.hpp>

namespace Vanta {

#ifndef VANTA_CUSTOM_LOG_SINKS
    extern void CreateLogSinks(Log::SinkList&) {}
#endif
}
