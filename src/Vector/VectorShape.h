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
		vertices.assign(data, data + size);
	}

	void SetIndices(const uint16_t* data, size_t size)
	{
		indices.assign(data, data + size);
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
	float mStrokeWidth = 0.0f;
	float mStrokeR = 0.0f;
	float mStrokeG = 0.0f;
	float mStrokeB = 0.0f;
	float mStrokeA = 0.0f;

	// Fill
	float mFillR = 0.0f;
	float mFillG = 0.0f;
	float mFillB = 0.0f;
	float mFillA = 0.0f;
};

//------------------------------------------------------------------------------
class Line : public IVectorShape
{
public:
	Line() = default;
	Line(float x1, float y1, float x2, float y2);

	virtual TessellationData Tessellate(IRenderDevice* renderDevice) const override;

	// Start point
	float mX1 = 0.0f;
	float mY1 = 0.0f;

	// End point
	float mX2 = 0.0f;
	float mY2 = 0.0f;
};

//------------------------------------------------------------------------------
class Rect : public IVectorShape
{
public:
	Rect() = default;
	Rect(float x, float y, float width, float height);

	virtual TessellationData Tessellate(IRenderDevice* renderDevice) const override;

	// Top-left
	float mX = 0.0f;
	float mY = 0.0f;

	// Size
	float mWidth = 0.0f;
	float mHeight = 0.0f;
};

//------------------------------------------------------------------------------
class BezierCurve : public IVectorShape
{
public:
	BezierCurve() = default;
	BezierCurve(float x1, float y1, float x2, float y2, float cx1, float cy1, float cx2, float cy2);

	virtual TessellationData Tessellate(IRenderDevice* renderDevice) const override;
	virtual void ComputeXY(float t, float& x, float& y) const;

	// Start point
	float mX1 = 0.0f;
	float mY1 = 0.0f;

	// End point
	float mX2 = 0.0f;
	float mY2 = 0.0f;

	// Control point 1
	float mCX1 = 0.0f;
	float mCY1 = 0.0f;

	// Control point 2
	float mCX2 = 0.0f;
	float mCY2 = 0.0f;
};

//------------------------------------------------------------------------------
class CubicBezierCurve : public BezierCurve
{
public:
	virtual void ComputeXY(float t, float& x, float& y) const override;
};
