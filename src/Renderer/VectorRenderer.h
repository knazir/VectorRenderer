#pragma once

// System
#include <vector>

//------------------------------------------------------------------------------
class IRenderDevice;
class IVectorShape;

//------------------------------------------------------------------------------
class VectorRenderer
{
public:
	VectorRenderer(IRenderDevice* renderer);

	void AddShape(const IVectorShape* shape);
	void ClearShapes();
	void Render();

private:
	IRenderDevice* mRenderDevice = nullptr;
	std::vector<const IVectorShape*> mShapes;
};

