#pragma once

// System
#include <stdint.h>

//------------------------------------------------------------------------------
enum class GraphicsBackend
{
	DirectX,
	OpenGL
};

//------------------------------------------------------------------------------
class IRenderDevice
{
public:
	virtual ~IRenderDevice() = default;

	// Lifecycle
	virtual bool Initialize(void* windowHandle, int32_t width, int32_t height) = 0;
	virtual void Resize(int32_t width, int32_t height) = 0;
	virtual void Clear(float r, float g, float b, float a) = 0;
	virtual void Render() = 0;
	virtual void Shutdown() = 0;

	// Rendering
	virtual void CreateVertexBuffer(const float* vertices, size_t size) = 0;
	virtual void CreateIndexBuffer(const uint16_t* indices, size_t size) = 0;
	virtual void SetVertexBuffer() = 0;
	virtual void SetIndexBuffer() = 0;
	virtual void DrawIndexedTriangles(size_t indexCount) = 0;
};

