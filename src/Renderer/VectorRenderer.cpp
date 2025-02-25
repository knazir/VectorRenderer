#include "VectorRenderer.h"

// Renderer
#include <Renderer/IRenderDevice.h>

// Vector
#include <Vector/VectorShape.h>

//------------------------------------------------------------------------------
VectorRenderer::VectorRenderer(IRenderDevice* renderer)
	: mRenderDevice(renderer)
{
}

//------------------------------------------------------------------------------
void VectorRenderer::AddShape(const IVectorShape* shape)
{
	mShapes.push_back(shape);
}

//------------------------------------------------------------------------------
void VectorRenderer::ClearShapes()
{
	for (const IVectorShape* shape : mShapes)
	{
		delete shape;
	}
	mShapes.clear();
}

//------------------------------------------------------------------------------
void VectorRenderer::Render()
{
	mRenderDevice->PreRender();

	for (const IVectorShape* shape : mShapes)
	{
		const TessellationData data = shape->Tessellate(mRenderDevice);
	
		mRenderDevice->CreateVertexBuffer(data.vertices.data(), data.vertices.size() * sizeof(Vertex));
		mRenderDevice->CreateIndexBuffer(data.indices.data(), data.indices.size() * sizeof(uint16_t));
		mRenderDevice->SetVertexBuffer();
		mRenderDevice->SetIndexBuffer();
		mRenderDevice->SetConstantBuffers();
	
		mRenderDevice->DrawIndexedTriangles(data.indices.size());
	}

	mRenderDevice->Render();
}