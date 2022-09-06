#pragma once
#include "Vanta/Render/UniformBuffer.hpp"

namespace Vanta {

	class OpenGLUniformBuffer : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(uint32 size, uint32 binding);
		virtual ~OpenGLUniformBuffer();

		virtual void SetData(const void* data, uint32 size, uint32 offset = 0) override;

	private:
		uint m_RendererID = 0;
	};
}
