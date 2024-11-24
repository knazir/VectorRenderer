#pragma once

#include "IRenderer.h"
#include "DirectXRenderer.h"

// External
#include <QDebug>

//------------------------------------------------------------------------------
class RendererFactory
{
public:
	static IRenderer* Create(GraphicsBackend type)
	{
		switch (type)
		{
		case GraphicsBackend::DirectX:
		{
			return new DirectXRenderer();
		}
		default:
		{
			qWarning() << "Unsupported renderer type";
			return nullptr;
		}
		}
	}
};
