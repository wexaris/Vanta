#include "vantapch.hpp"
#include "Vanta/Render/RenderCommand.hpp"

namespace Vanta {

    Box<GraphicsAPI> RenderCommand::s_GraphicsAPI = GraphicsAPI::Create();
}
