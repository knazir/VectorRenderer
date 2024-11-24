#pragma once

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
	virtual void Clear(float r, float g, float b, float a) = 0;
	virtual void Render() = 0;
	virtual void Shutdown() = 0;

	// Resources
	virtual bool LoadVertexShader(const std::wstring& filePath, const std::string& entryPoint) = 0;
	virtual bool LoadPixelShader(const std::wstring& filePath, const std::string& entryPoint) = 0;

	// Rendering
	virtual void CreateVertexBuffer(const Vertex* vertices, size_t size) = 0;
	virtual void CreateIndexBuffer(const uint16_t* indices, size_t size) = 0;
	virtual void SetVertexBuffer() = 0;
	virtual void SetIndexBuffer() = 0;
	virtual void DrawIndexedTriangles(size_t indexCount) = 0;
};

