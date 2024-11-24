#include "VectorShape.h"

// System
#include <stdint.h>
#include <cmath>

//------------------------------------------------------------------------------
/*virtual*/ void IVectorShape::SetStroke(float r, float g, float b, float a, float width)
{
	mStrokeR = r;
	mStrokeG = g;
	mStrokeB = b;
	mStrokeA = a;
}

//------------------------------------------------------------------------------
/*virtual*/ void IVectorShape::SetFill(float r, float g, float b, float a)
{
	mFillR = r;
	mFillG = g;
	mFillB = b;
	mFillA = a;
}

//------------------------------------------------------------------------------
Line::Line(float x1, float y1, float x2, float y2)
	: mX1(x1)
	, mY1(y1)
	, mX2(x2)
	, mY2(y2)
{
}

//------------------------------------------------------------------------------
/*virtual*/ TessellationData Line::Tessellate(IRenderDevice* renderDevice) const
{
	TessellationData data;

	float halfWidth = mStrokeWidth * 0.5f;

	// Calculate the normal for the line's width
	float dx = mX2 - mX1;
	float dy = mY2 - mY1;
	float length = std::sqrt(dx * dx + dy + dy);
	dx /= length;
	dy /= length;

	// Normal vectors for width
	const float px = -dy * halfWidth;
	const float py = dx * halfWidth;

	// Four corners of the quad
	const Vertex vertices[] =
	{
		Vertex(mX1 + px, mY1 + py, 0.0f, mStrokeR, mStrokeG, mStrokeB, mStrokeA),
		Vertex(mX1 - px, mY1 - py, 0.0f, mStrokeR, mStrokeG, mStrokeB, mStrokeA),
		Vertex(mX2 + px, mY2 + py, 0.0f, mStrokeR, mStrokeG, mStrokeB, mStrokeA),
		Vertex(mX2 - px, mY2 - py, 0.0f, mStrokeR, mStrokeG, mStrokeB, mStrokeA)
	};
	data.SetVertices(vertices, sizeof(vertices));

	// Indices for the two triangles
	uint16_t indices[] = { 0, 1, 2, 1, 3, 2 };
	data.SetIndices(indices, sizeof(indices));

	return data;
}

//------------------------------------------------------------------------------
Rect::Rect(float x, float y, float width, float height)
	: mX(x)
	, mY(y)
	, mWidth(width)
	, mHeight(height)
{
}

//------------------------------------------------------------------------------
/*virtual*/ TessellationData Rect::Tessellate(IRenderDevice* renderDevice) const
{
	TessellationData data;

	const float x2 = mX + mWidth;
	const float y2 = mY + mHeight;

	// Four corners of the rectangle
	const Vertex vertices[] =
	{
		Vertex(mX, y2, 0.0f, mFillR, mFillG, mFillB, mFillA),	// Bottom-left
		Vertex(x2, y2, 0.0f, mFillR, mFillG, mFillB, mFillA),	// Bottom-right
		Vertex(x2, mY, 0.0f, mFillR, mFillG, mFillB, mFillA),	// Top-right
		Vertex(mX, mY, 0.0f, mFillR, mFillG, mFillB, mFillA),	// Top-left
	};
	data.SetVertices(vertices, sizeof(vertices));

	// Indices for the two triangles
	uint16_t indices[] = { 0, 1, 2, 0, 2, 3 };
	data.SetIndices(indices, sizeof(indices));

	return data;
}

//------------------------------------------------------------------------------
BezierCurve::BezierCurve(float x1, float y1, float x2, float y2, float cx1, float cy1, float cx2, float cy2)
	: mX1(x1)
	, mY1(y1)
	, mX2(x2)
	, mY2(y2)
	, mCX1(cx1)
	, mCY1(cy1)
	, mCX2(cx2)
	, mCY2(cy2)
{
}

//------------------------------------------------------------------------------
/*virtual*/ TessellationData BezierCurve::Tessellate(IRenderDevice* renderDevice) const
{
	TessellationData data;

	static const int kSegments = 20;	// TODO: Allow this to be set for quality/performance tradeoff
	Vertex vertices[kSegments];			// Position + Fill color
	uint16_t indices[kSegments * 2];	// Two triangles per segment
	
	int32_t vertexIndex = 0;
	int32_t indexIndex = 0;

	for (int32_t i = 0; i < kSegments; ++i)
	{
		const float t = (float)i / kSegments;
		float x = 0.0f;
		float y = 0.0f;
		ComputeXY(t, x, y);

		// Store vertex
		vertices[vertexIndex++] = Vertex(x, y, 0.0f, mStrokeR, mStrokeG, mStrokeB, mStrokeA);

		// Store indices
		if (i > 0)
		{
			indices[indexIndex++] = i - 1;
			indices[indexIndex++] = i;
		}
	}

	data.SetVertices(vertices, sizeof(vertices));
	data.SetIndices(indices, sizeof(indices));

	return data;
}

//------------------------------------------------------------------------------
void BezierCurve::ComputeXY(float t, float& x, float& y) const
{
	// Quadratic Bezier interpolation (De Casteljau's algorithm)
	x = (1.0f - t) * (1.0f - t) * mX1 + 2 * (1.0f - t) * t * mCX1 + t * t + mX2;
	y = (1.0f - t) * (1.0f - t) * mY1 + 2 * (1.0f - t) * t * mCY1 + t * t * mY2;
}

//------------------------------------------------------------------------------
/*virtual*/ void CubicBezierCurve::ComputeXY(float t, float& x, float& y) const
{
	// Cubic Bezier interpolation (De Casteljau's algorithm)
	x = (1.0f - t) * (1.0f - t) * (1.0f - t) * mX1
	  + 3 * (1.0f - t) * (1.0f - t) * t * mCX1
	  + 3 * (1.0f - t) * t * t * mCX2 + t * t * t * mX2;
	y = (1.0f - t) * (1.0f - t) * (1.0f - t) * mY1
	  + 3 * (1.0f - t) * (1.0f - t) * t * mCY1
	  + 3 * (1.0f - t) * t * t * mCY2
	  + t * t * t * mY2;
}

