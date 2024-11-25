#include "DirectXRenderDevice.h"

// Utils
#include <Utils/Assert.h>

// External
#include <d3dcompiler.h>
#include <External/Eigen/Dense>
#include <External/Eigen/Geometry>
#include <QDebug>

//------------------------------------------------------------------------------
struct ConstantBuffer
{
	Eigen::Matrix4f worldViewProj;
};

//------------------------------------------------------------------------------
#define RELEASE(x) if ((x)) { (x)->Release(); (x) = nullptr; }

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

	// Setup rasterizer state
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE; // TODO: This seems to break some bezier curves
	//rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthClipEnable = TRUE;
	hr = mDevice->CreateRasterizerState(&rasterizerDesc, &mRasterizerState);
	if (FAILED(hr))
	{
		ASSERT(false, "Failed to create rasterizer state");
		return false;
	}
	mDeviceContext->RSSetState(mRasterizerState);

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

	mDeviceContext->OMSetRenderTargets(1u, &mRenderTargetView, nullptr);
	UpdateViewport(width, height);

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

	HRESULT hr = mSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
	if (FAILED(hr))
	{
		ASSERT(false, "Failed to resize swap chain buffers");
		return;
	}

	ID3D11Texture2D* backBuffer = nullptr;
	hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	if (FAILED(hr))
	{
		ASSERT(false, "Failed to get back buffer");
		return;
	}


	hr = mDevice->CreateRenderTargetView(backBuffer, nullptr, &mRenderTargetView);
	backBuffer->Release();
	if (FAILED(hr))
	{
		ASSERT(false, "Failed to recreate render target view");
		return;
	}

	mDeviceContext->OMSetRenderTargets(1u, &mRenderTargetView, nullptr);
	UpdateViewport(width, height);
}

//------------------------------------------------------------------------------
/*virtual*/ void DirectXRenderDevice::PreRender()
{
	float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	mDeviceContext->ClearRenderTargetView(mRenderTargetView, clearColor);
	mDeviceContext->OMSetRenderTargets(1u, &mRenderTargetView, nullptr);
}

//------------------------------------------------------------------------------
/*virtual*/ void DirectXRenderDevice::Render()
{
	// Swap the back and front buffers
	mSwapChain->Present(1u, 0u);
}

//------------------------------------------------------------------------------
/*virtual*/ void DirectXRenderDevice::Shutdown()
{
	CleanupRenderTarget();

	RELEASE(mVertexShader);
	RELEASE(mPixelShader);
	RELEASE(mVertexBuffer);
	RELEASE(mIndexBuffer);
	RELEASE(mInputLayout);
	RELEASE(mRasterizerState);
	RELEASE(mSwapChain);
	RELEASE(mDeviceContext);
	RELEASE(mDevice);
}

/*virtual*/ bool DirectXRenderDevice::LoadShaders()
{
	ID3DBlob* vertexShaderBlob = LoadVertexShader(kShadersDir + L"VertexShader.hlsl", "main");
	if (vertexShaderBlob == nullptr)
	{
		return false;
	}

	ID3DBlob* pixelShaderBlob = LoadPixelShader(kShadersDir + L"PixelShader.hlsl", "main");
	if (pixelShaderBlob == nullptr)
	{
		return false;
	}

	SetInputLayout(vertexShaderBlob);

	vertexShaderBlob->Release();
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
/*virtrual*/ void DirectXRenderDevice::SetConstantBuffers()
{
	using namespace Eigen;

	const float left = -1.0f;
	const float right = 1.0f;
	const float top = -1.0f;
	const float bottom = 1.0f;

	Matrix4f projection = Matrix4f::Identity();
	projection(0, 0) = 2.0f / (right - left);
	projection(1, 1) = 2.0f / (top - bottom);
	projection(0, 3) = -(right + left) / (right - left);
	projection(1, 3) = -(top + bottom) / (top - bottom);

	// Create constant buffer
	ConstantBuffer cb;
	cb.worldViewProj = projection.inverse();

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0u;
	bufferDesc.MiscFlags = 0u;
	bufferDesc.ByteWidth = sizeof(ConstantBuffer);

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = &cb;

	ID3D11Buffer* constantBuffer = nullptr;
	HRESULT hr = mDevice->CreateBuffer(&bufferDesc, &initData, &constantBuffer);
	if (FAILED(hr))
	{
		ASSERT(false, "Failed to create constant buffer");
	}

	mDeviceContext->VSSetConstantBuffers(0u, 1u, &constantBuffer);

	constantBuffer->Release();
}

//------------------------------------------------------------------------------
/*virtual*/ void DirectXRenderDevice::DrawIndexedTriangles(size_t indexCount)
{
	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mDeviceContext->DrawIndexed(static_cast<UINT>(indexCount), 0, 0);
}

//------------------------------------------------------------------------------
void DirectXRenderDevice::UpdateViewport(float width, float height)
{
	mWidth = width;
	mHeight = height;

	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<FLOAT>(width);
	viewport.Height = static_cast<FLOAT>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	mDeviceContext->RSSetViewports(1u, &viewport);
}

//------------------------------------------------------------------------------
void DirectXRenderDevice::CleanupRenderTarget()
{
	if (mRenderTargetView != nullptr)
	{
		mRenderTargetView->Release();
		mRenderTargetView = nullptr;
	}
}

//------------------------------------------------------------------------------
ID3DBlob* DirectXRenderDevice::LoadVertexShader(const std::wstring& filePath, const std::string& entryPoint)
{
	ID3DBlob* vertexShaderBlob = CompileShader(filePath, entryPoint, "vs_5_0");
	if (vertexShaderBlob == nullptr)
	{
		ASSERT(false, "Failed to compile vertex shader");
		return nullptr;
	}

	HRESULT hr = mDevice->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &mVertexShader);
	if (FAILED(hr))
	{
		ASSERT(false, "Failed to create vertex shader");
		return nullptr;
	}

	mDeviceContext->VSSetShader(mVertexShader, nullptr, 0);

	// Caller must release blob
	return vertexShaderBlob;
}

//------------------------------------------------------------------------------
ID3DBlob* DirectXRenderDevice::LoadPixelShader(const std::wstring& filePath, const std::string& entryPoint)
{
	ID3DBlob* pixelShaderBlob = CompileShader(filePath, entryPoint, "ps_5_0");
	if (pixelShaderBlob == nullptr)
	{
		ASSERT(false, "Failed to compile pixel shader");
		return nullptr;
	}

	HRESULT hr = mDevice->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &mPixelShader);
	if (FAILED(hr))
	{
		ASSERT(false, "Failed to create pixel shader");
		return nullptr;
	}

	mDeviceContext->PSSetShader(mPixelShader, nullptr, 0);

	// Caller must release blob
	return pixelShaderBlob;
}

//------------------------------------------------------------------------------
void DirectXRenderDevice::SetInputLayout(ID3DBlob* vertexShaderBlob)
{
	// This really assumes that the vertex format is always the same
	// TODO: Make it configurable for other shader inputs?
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",	0,	DXGI_FORMAT_R32G32B32_FLOAT,	0,	offsetof(Vertex, x),	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "COLOR",		0,	DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	offsetof(Vertex, r),	D3D11_INPUT_PER_VERTEX_DATA,	0 }
	};

	HRESULT hr = mDevice->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &mInputLayout);
	if (FAILED(hr))
	{
		ASSERT(false, "Failed to create input layout");
	}

	mDeviceContext->IASetInputLayout(mInputLayout);
}

//------------------------------------------------------------------------------
ID3DBlob* DirectXRenderDevice::CompileShader(const std::wstring& filePath, const std::string& entryPoint, const std::string& target)
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

	if (errorBlob != nullptr)
	{
		errorBlob->Release();
		errorBlob = nullptr;
	}

	return shaderBlob;
}
