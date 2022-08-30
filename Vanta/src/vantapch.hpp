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
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

// Core
#include "Vanta/Core/Definitions.hpp"
#include "Vanta/Core/Types.hpp"
#include "Vanta/Core/Log.hpp"
#include "Vanta/Core/Fibers.hpp"
#include "Vanta/Core/UUID.hpp"

// Util
#include "Vanta/Util/Math.hpp"
#include "Vanta/Util/Util.hpp"

// Telemetry
#include "Vanta/Debug/Instrumentor.hpp"

// IO
#include "Vanta/IO/File.hpp"
#include "Vanta/IO/Image.hpp"
