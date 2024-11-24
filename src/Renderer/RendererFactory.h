#pragma once

#include "IRenderDevice.h"
#include "DirectXRenderDevice.h"

// Utils
#include <Utils/Assert.h>

//------------------------------------------------------------------------------
class RendererFactory
{
public:
	static IRenderDevice* Create(GraphicsBackend type)
	{
		switch (type)
		{
		case GraphicsBackend::DirectX:
		{
			return new DirectXRenderDevice();
		}
		default:
		{
			ASSERT(false, "Unsupported graphics backend");
			return nullptr;
		}
		}
	}
};
