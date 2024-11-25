#pragma once

// Renderer
#include <Renderer/IRenderDevice.h>

// System
#include <vector>

//------------------------------------------------------------------------------
struct TessellationData
{
	std::vector<Vertex> vertices;	// Position, color
	std::vector<uint16_t> indices;	// Triangle indices

	void SetVertices(const Vertex* data, size_t size)
	{
		vertices.assign(data, (const Vertex*)((const char*)data + size));
	}

	void SetIndices(const uint16_t* data, size_t size)
	{
		indices.assign(data, (const uint16_t*)((const char*)data + size));
	}

	void Normalize()
	{
		for (Vertex& vertex : vertices)
		{
			vertex.Normalize();
		}
	}
};

//------------------------------------------------------------------------------
class IVectorShape
{
public:
	virtual ~IVectorShape() = default;

	virtual TessellationData Tessellate(IRenderDevice* renderDevice) const = 0;

	virtual void SetStroke(float r, float g, float b, float a, float width);
	virtual void SetFill(float r, float g, float b, float a);

	// Stroke
	float strokeWidth = 0.0f;
	float strokeR = 0.0f;
	float strokeG = 0.0f;
	float strokeB = 0.0f;
	float strokeA = 0.0f;

	// Fill
	float fillR = 0.0f;
	float fillG = 0.0f;
	float fillB = 0.0f;
	float fillA = 0.0f;
};

//------------------------------------------------------------------------------
class Line : public IVectorShape
{
public:
	Line() = default;
	Line(float x1, float y1, float x2, float y2);

	virtual TessellationData Tessellate(IRenderDevice* renderDevice) const override;

	// Start point
	float x1 = 0.0f;
	float y1 = 0.0f;

	// End point
	float x2 = 0.0f;
	float y2 = 0.0f;
};

//------------------------------------------------------------------------------
class Rect : public IVectorShape
{
public:
	Rect() = default;
	Rect(float x, float y, float width, float height);

	virtual TessellationData Tessellate(IRenderDevice* renderDevice) const override;

	// Top-left
	float x = 0.0f;
	float y = 0.0f;

	// Size
	float width = 0.0f;
	float height = 0.0f;
};

//------------------------------------------------------------------------------
class BezierCurve : public IVectorShape
{
public:
	BezierCurve() = default;
	BezierCurve(float x1, float y1, float x2, float y2, float cx1, float cy1);

	virtual TessellationData Tessellate(IRenderDevice* renderDevice) const override;
	virtual void ComputeXY(float t, float& x, float& y) const;

	// Start point
	float x1 = 0.0f;
	float y1 = 0.0f;

	// End point
	float x2 = 0.0f;
	float y2 = 0.0f;

	// Control point 1
	float cx1 = 0.0f;
	float cy1 = 0.0f;
};

//------------------------------------------------------------------------------
class CubicBezierCurve : public BezierCurve
{
public:
	CubicBezierCurve(float x1, float y1, float x2, float y2, float cx1, float cy1, float cx2, float cy2);

	virtual void ComputeXY(float t, float& x, float& y) const override;

	// Control point 2
	float cx2 = 0.0f;
	float cy2 = 0.0f;
};
