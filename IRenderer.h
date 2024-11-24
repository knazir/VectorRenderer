#pragma once

#include <stdint.h>

//------------------------------------------------------------------------------
enum class GraphicsBackend
{
	DirectX,
	OpenGL
};

//------------------------------------------------------------------------------
class IRenderer
{
public:
	virtual ~IRenderer() = default;

	virtual bool Initialize(void* windowHandle, int32_t width, int32_t height) = 0;
	virtual void Resize(int32_t width, int32_t height) = 0;
	virtual void Clear(float r, float g, float b, float a) = 0;
	virtual void Render() = 0;
	virtual void Shutdown() = 0;
};

