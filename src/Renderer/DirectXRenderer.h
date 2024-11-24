#pragma once

#include "IRenderer.h"

// External
#include <d3d11.h>

//------------------------------------------------------------------------------
class DirectXRenderer : public IRenderer
{
public:
	DirectXRenderer();
	~DirectXRenderer();

	virtual bool Initialize(void* windowHandle, int32_t width, int32_t height) override;
	virtual void Resize(int32_t width, int32_t height) override;
	virtual void Clear(float r, float g, float b, float a) override;
	virtual void Render() override;
	virtual void Shutdown() override;

private:
	void CleanupRenderTarget();

	HWND mHWND = 0;
	ID3D11Device* mDevice = nullptr;
	ID3D11DeviceContext* mDeviceContext = nullptr;
	IDXGISwapChain* mSwapChain = nullptr;
	ID3D11RenderTargetView* mRenderTargetView = nullptr;
};

