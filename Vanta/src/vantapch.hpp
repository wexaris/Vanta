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
#include "Vanta/Config.hpp"
#include "Vanta/Base/Platform.hpp"
#include "Vanta/Base/Definitions.hpp"
#include "Vanta/Base/Types.hpp"
#include "Vanta/Core/Assert.hpp"
#include "Vanta/Core/Buffer.hpp"
#include "Vanta/Core/Log.hpp"
#include "Vanta/Core/Fibers.hpp"
#include "Vanta/Core/UUID.hpp"

// Util
#include "Vanta/Util/Format.hpp"

// Telemetry
#include "Vanta/Debug/Instrumentor.hpp"

// IO
#include "Vanta/IO/IO.hpp"

// Input
#include "Vanta/Input/Input.hpp"
