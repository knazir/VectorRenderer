#include "DirectXRenderDevice.h"

// Utils
#include <Utils/Assert.h>

// External
#include <QDebug>
#include <d3dcompiler.h>

//------------------------------------------------------------------------------
DirectXRenderDevice::DirectXRenderDevice()
{
}

//------------------------------------------------------------------------------
DirectXRenderDevice::~DirectXRenderDevice()
{
	Shutdown();
}

//------------------------------------------------------------------------------
/*virtual*/ bool DirectXRenderDevice::Initialize(void* windowHandle, int32_t width, int32_t height)
{
	mHWND = static_cast<HWND>(windowHandle);

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Width = width;
	swapChainDesc.BufferDesc.Height = height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = mHWND;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
#ifdef DEBUG
		D3D11_CREATE_DEVICE_DEBUG,
#else
		0,
#endif
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
		ASSERT(false, "Failed to create device and swap chain");
		return false;
	}

	// Create render target view
	ID3D11Texture2D* backBuffer = nullptr;
	hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	if (FAILED(hr))
	{
		ASSERT(false, "Failed to get back buffer");
		return false;
	}
	mDevice->CreateRenderTargetView(backBuffer, nullptr, &mRenderTargetView);
	backBuffer->Release();

	mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, nullptr);

	return true;
}

//------------------------------------------------------------------------------
/*virtual*/ void DirectXRenderDevice::Resize(int32_t width, int32_t height)
{
	if (mSwapChain == nullptr)
	{
		return;
	}

	CleanupRenderTarget();
	mSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);

	ID3D11Texture2D* backBuffer = nullptr;
	const HRESULT hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	if (FAILED(hr))
	{
		ASSERT(false, "Failed to get back buffer");
		return;
	}
	mDevice->CreateRenderTargetView(backBuffer, nullptr, &mRenderTargetView);
	backBuffer->Release();

	mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, nullptr);
}

//------------------------------------------------------------------------------
/*virtual*/ void DirectXRenderDevice::Clear(float r, float g, float b, float a)
{
	float clearColor[] = { r, g, b, a };
	mDeviceContext->ClearRenderTargetView(mRenderTargetView, clearColor);
}

//------------------------------------------------------------------------------
/*virtual*/ void DirectXRenderDevice::Render()
{
	// Swap the back and front buffers
	mSwapChain->Present(0, 0);
}

//------------------------------------------------------------------------------
/*virtual*/ void DirectXRenderDevice::Shutdown()
{
	CleanupRenderTarget();

	if (mVertexShader != nullptr)
	{
		mVertexShader->Release();
		mVertexShader = nullptr;
	}

	if (mPixelShader != nullptr)
	{
		mPixelShader->Release();
		mPixelShader = nullptr;
	}

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
/*virtual*/ bool DirectXRenderDevice::LoadVertexShader(const std::wstring& filePath, const std::string& entryPoint)
{
	ID3DBlob* vertexShaderBlob = CompileShader(filePath, entryPoint, "vs_5_0");
	if (vertexShaderBlob == nullptr)
	{
		ASSERT(false, "Failed to compile vertex shader");
		return false;
	}

	HRESULT hr = mDevice->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &mVertexShader);
	if (FAILED(hr))
	{
		ASSERT(false, "Failed to create vertex shader");
		return false;
	}

	mDeviceContext->VSSetShader(mVertexShader, nullptr, 0);
	vertexShaderBlob->Release();

	return true;
}

//------------------------------------------------------------------------------
/*virtual*/ bool DirectXRenderDevice::LoadPixelShader(const std::wstring& filePath, const std::string& entryPoint)
{
	ID3DBlob* pixelShaderBlob = CompileShader(filePath, entryPoint, "ps_5_0");
	if (pixelShaderBlob == nullptr)
	{
		ASSERT(false, "Failed to compile pixel shader");
		return false;
	}

	HRESULT hr = mDevice->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &mPixelShader);
	if (FAILED(hr))
	{
		ASSERT(false, "Failed to create pixel shader");
		return false;
	}

	mDeviceContext->PSSetShader(mPixelShader, nullptr, 0);
	pixelShaderBlob->Release();

	return true;
}

//------------------------------------------------------------------------------
/*virtual*/ void DirectXRenderDevice::CreateVertexBuffer(const Vertex* vertices, size_t size)
{
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0u;
	bufferDesc.MiscFlags = 0u;
	bufferDesc.ByteWidth = static_cast<UINT>(size);

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices;

	HRESULT hr = mDevice->CreateBuffer(&bufferDesc, &initData, &mVertexBuffer);
	if (FAILED(hr))
	{
		ASSERT(false, "Failed to create vertex buffer");
	}
}

//------------------------------------------------------------------------------
/*virtual*/ void DirectXRenderDevice::CreateIndexBuffer(const uint16_t* indices, size_t size)
{
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0u;
	bufferDesc.MiscFlags = 0u;
	bufferDesc.ByteWidth = static_cast<UINT>(size);

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = indices;

	HRESULT hr = mDevice->CreateBuffer(&bufferDesc, &initData, &mIndexBuffer);
	if (FAILED(hr))
	{
		ASSERT(false, "Failed to create index buffer");
	}
}

//------------------------------------------------------------------------------
/*virtual*/ void DirectXRenderDevice::SetVertexBuffer()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	mDeviceContext->IASetVertexBuffers(0u, 1u, &mVertexBuffer, &stride, &offset);
}

//------------------------------------------------------------------------------
/*virtual*/ void DirectXRenderDevice::SetIndexBuffer()
{
	mDeviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
}

//------------------------------------------------------------------------------
/*virtual*/ void DirectXRenderDevice::DrawIndexedTriangles(size_t indexCount)
{
	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mDeviceContext->DrawIndexed(static_cast<UINT>(indexCount), 0, 0);
}

//------------------------------------------------------------------------------
/*virtual*/ void DirectXRenderDevice::CleanupRenderTarget()
{
	if (mRenderTargetView != nullptr)
	{
		mRenderTargetView->Release();
		mRenderTargetView = nullptr;
	}
}

//------------------------------------------------------------------------------
/*virtual*/ ID3DBlob* DirectXRenderDevice::CompileShader(const std::wstring& filePath, const std::string& entryPoint, const std::string& target)
{
	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	HRESULT hr = D3DCompileFromFile(
		filePath.c_str(),				// File path to HLSL shader
		nullptr,						// Optional macros
		nullptr,						// Include handler (nullptr for default)
		entryPoint.c_str(),				// Shader entry point
		target.c_str(),					// Shader model (e.g., "vs_5_0" or "ps_5_0")
		D3DCOMPILE_ENABLE_STRICTNESS,	// Compilation flags
		0,								// Effect flags
		&shaderBlob,					// Compiled shader blob
		&errorBlob						// Error messages
	);

	if (FAILED(hr))
	{
		if (errorBlob != nullptr)
		{
			qWarning() << "Shader compilation error: " << static_cast<const char*>(errorBlob->GetBufferPointer());
		}
		ASSERT(false, "Failed to compile shader");
		return nullptr;
	}

	return shaderBlob;
}
