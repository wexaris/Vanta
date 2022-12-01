#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <fstream>
#include <memory>
#include <optional>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <utility>
#include <vector>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

// Core
#include "Vanta/Core/Config.hpp"
#include "Vanta/Core/Definitions.hpp"
#include "Vanta/Core/Types.hpp"
#include "Vanta/Core/Buffer.hpp"
#include "Vanta/Core/Log.hpp"
#include "Vanta/Core/Fibers.hpp"
#include "Vanta/Core/UUID.hpp"

// Util
#include "Vanta/Util/Util.hpp"
#include "Vanta/Util/Math.hpp"

// Telemetry
#include "Vanta/Debug/Instrumentor.hpp"

// IO
#include "Vanta/IO/IO.hpp"
