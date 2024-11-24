#pragma once

#include "IRenderDevice.h"
#include "DirectXRenderDevice.h"

// External
#include <QDebug>

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
			qWarning() << "Unsupported renderer type";
			return nullptr;
		}
		}
	}
};
