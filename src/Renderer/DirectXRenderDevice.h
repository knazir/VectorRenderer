#pragma once

#include "IRenderDevice.h"

// External
#include <d3d11.h>

// System
#include <string>

//------------------------------------------------------------------------------
class DirectXRenderDevice : public IRenderDevice
{
public:
	DirectXRenderDevice();
	~DirectXRenderDevice();

	virtual bool Initialize(void* windowHandle, int32_t width, int32_t height) override;
	virtual void Resize(int32_t width, int32_t height) override;
	virtual void Clear(float r, float g, float b, float a) override;
	virtual void Render() override;
	virtual void Shutdown() override;

	virtual bool LoadVertexShader(const std::wstring& filePath, const std::string& entryPoint) override;
	virtual bool LoadPixelShader(const std::wstring& filePath, const std::string& entryPoint) override;

	virtual void CreateVertexBuffer(const Vertex* vertices, size_t size) override;
	virtual void CreateIndexBuffer(const uint16_t* indices, size_t size) override;
	virtual void SetVertexBuffer() override;
	virtual void SetIndexBuffer() override;
	virtual void DrawIndexedTriangles(size_t indexCount) override;

private:
	void CleanupRenderTarget();
	ID3DBlob* CompileShader(const std::wstring& filePath, const std::string& entryPoint, const std::string& target);

	HWND mHWND = 0;
	ID3D11Device* mDevice = nullptr;
	ID3D11DeviceContext* mDeviceContext = nullptr;
	IDXGISwapChain* mSwapChain = nullptr;
	ID3D11RenderTargetView* mRenderTargetView = nullptr;

	ID3D11VertexShader* mVertexShader = nullptr;
	ID3D11PixelShader* mPixelShader = nullptr;
	ID3D11Buffer* mVertexBuffer = nullptr;
	ID3D11Buffer* mIndexBuffer = nullptr;
};

