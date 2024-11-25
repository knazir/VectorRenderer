#pragma once

// Utils
#include <Utils/Config.h>

// System
#include <stdint.h>
#include <string>

//------------------------------------------------------------------------------
static const std::wstring kShadersDir = L"C:\\Users\\Kijou\\Development\\Graphics\\VectorRenderer\\shaders\\";

//------------------------------------------------------------------------------
enum class GraphicsBackend
{
	DirectX,
	OpenGL
};

// Corresponds to the input parameters to BasicVertexShader
//------------------------------------------------------------------------------
struct Vertex
{
	Vertex() = default;
	Vertex(float x, float y, float z, float r, float g, float b, float a)
		: x(x)
		, y(y)
		, z(z)
		, r(r)
		, g(g)
		, b(b)
		, a(a)
	{
	}

	void Normalize()
	{
		// Translate to origin being top-left and double since NDC is -1 to 1
		x = ((x * 2.0f) / AUTHORED_WIDTH) - 1.0f;
		y = ((y * 2.0f) / AUTHORED_HEIGHT) - 1.0f;
	}

	// Position
	float x = 0.0f;
	float y = 0.0f;;
	float z = 0.0f;

	// Color
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
	float a = 0.0f;
};

//------------------------------------------------------------------------------
class IRenderDevice
{
public:
	virtual ~IRenderDevice() = default;

	// Lifecycle
	virtual bool Initialize(void* windowHandle, int32_t width, int32_t height) = 0;
	virtual void Resize(int32_t width, int32_t height) = 0;
	virtual void PreRender() = 0;
	virtual void Render() = 0;
	virtual void Shutdown() = 0;

	// Resources
	virtual bool LoadShaders() = 0;

	// Rendering
	virtual void CreateVertexBuffer(const Vertex* vertices, size_t size) = 0;
	virtual void CreateIndexBuffer(const uint16_t* indices, size_t size) = 0;
	virtual void SetVertexBuffer() = 0;
	virtual void SetIndexBuffer() = 0;
	virtual void SetConstantBuffers() = 0;
	virtual void DrawIndexedTriangles(size_t indexCount) = 0;
};

