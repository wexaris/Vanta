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

// Base
#include "Vanta/Config.hpp"
#include "Vanta/Base/Platform.hpp"
#include "Vanta/Base/Definitions.hpp"
#include <Vanta/Base/Types.hpp>

// Core definitions
#include "Vanta/Core/Assert.hpp"
#include <Vanta/Core/Buffer.hpp>
#include <Vanta/Core/Log.hpp>
#include <Vanta/Core/Fibers.hpp>
#include <Vanta/Core/UUID.hpp>

// Formatting
#include <Vanta/Util/Format.hpp>

// Telemetry
#include <Vanta/Debug/Instrumentor.hpp>

// Core
#include <Vanta/Core/Engine.hpp>
#include <Vanta/Core/Window.hpp>

// IO
#include "Vanta/IO/IO.hpp"

// Render
#include <Vanta/Render/Renderer.hpp>
#include <Vanta/Render/Renderer2D.hpp>

// Entity
#include <Vanta/Scene/Entity.hpp>
#include <Vanta/Scene/Scene.hpp>

// Scripts
#include <Vanta/Scripts/CSharp/ScriptEngine.hpp>
#include <Vanta/Scripts/Native/ScriptEngine.hpp>
