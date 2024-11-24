#include "DirectXRenderer.h"

#include <QDebug>

//------------------------------------------------------------------------------
DirectXRenderer::DirectXRenderer()
{
}

//------------------------------------------------------------------------------
DirectXRenderer::~DirectXRenderer()
{
	Shutdown();
}

//------------------------------------------------------------------------------
bool DirectXRenderer::Initialize(void* windowHandle, int32_t width, int32_t height)
{
	mHWND = static_cast<HWND>(windowHandle);

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = width;
	swapChainDesc.BufferDesc.Height = height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = mHWND;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&mSwapChain,
		&mDevice,
		nullptr,
		&mDeviceContext
	);

	if (FAILED(hr))
	{
		qWarning() << "Failed to create device and swap chain";
		return false;
	}

	// Create render target view
	ID3D11Texture2D* backBuffer = nullptr;
	hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	if (FAILED(hr))
	{
		qWarning() << "Failed to get back buffer";
		return false;
	}
	mDevice->CreateRenderTargetView(backBuffer, nullptr, &mRenderTargetView);
	backBuffer->Release();

	mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, nullptr);

	return true;
}

//------------------------------------------------------------------------------
void DirectXRenderer::Resize(int32_t width, int32_t height)
{
	if (mSwapChain == nullptr)
	{
		return;
	}

	CleanupRenderTarget();
	mSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);

	ID3D11Texture2D* backBuffer = nullptr;
	mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	mDevice->CreateRenderTargetView(backBuffer, nullptr, &mRenderTargetView);
	backBuffer->Release();

	mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, nullptr);
}

//------------------------------------------------------------------------------
void DirectXRenderer::Clear(float r, float g, float b, float a)
{
	float clearColor[] = { r, g, b, a };
	mDeviceContext->ClearRenderTargetView(mRenderTargetView, clearColor);
}

//------------------------------------------------------------------------------
void DirectXRenderer::Render()
{
	// Swap the back and front buffers
	mSwapChain->Present(1, 0);
}

//------------------------------------------------------------------------------
void DirectXRenderer::Shutdown()
{
	CleanupRenderTarget();

	if (mSwapChain != nullptr)
	{
		mSwapChain->Release();
		mSwapChain = nullptr;
	}

	if (mDeviceContext != nullptr)
	{
		mDeviceContext->Release();
		mDeviceContext = nullptr;
	}

	if (mDevice != nullptr)
	{
		mDevice->Release();
		mDevice = nullptr;
	}
}

//------------------------------------------------------------------------------
void DirectXRenderer::CleanupRenderTarget()
{
	if (mRenderTargetView != nullptr)
	{
		mRenderTargetView->Release();
		mRenderTargetView = nullptr;
	}
}
