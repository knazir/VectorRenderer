#include "VectorShape.h"

// Utils
#include <Utils/Config.h>

// External
#include <External/Eigen/Dense>

// System
#include <stdint.h>
#include <cmath>

namespace
{
//------------------------------------------------------------------------------
float GetNormalizedX(float x)
{
	// Translate to origin being top-left and double since NDC is -1 to 1
	return ((x * 2.0f) / AUTHORED_WIDTH) - 1.0f;
}

//------------------------------------------------------------------------------
float GetNormalizedY(float y)
{
	// Translate to origin being top-left and double since NDC is -1 to 1
	return ((y * 2.0f) / AUTHORED_HEIGHT) - 1.0f;
}
}


//------------------------------------------------------------------------------
/*virtual*/ void IVectorShape::SetStroke(float r, float g, float b, float a, float width)
{
	strokeR = r;
	strokeG = g;
	strokeB = b;
	strokeA = a;
	strokeWidth = width;
}

//------------------------------------------------------------------------------
/*virtual*/ void IVectorShape::SetFill(float r, float g, float b, float a)
{
	fillR = r;
	fillG = g;
	fillB = b;
	fillA = a;
}

//------------------------------------------------------------------------------
Line::Line(float x1, float y1, float x2, float y2)
	: x1(x1)
	, y1(y1)
	, x2(x2)
	, y2(y2)
{
}

//------------------------------------------------------------------------------
/*virtual*/ TessellationData Line::Tessellate(IRenderDevice* renderDevice) const
{
	using namespace Eigen;

	TessellationData data;

	float halfWidth = strokeWidth * 0.5f;

	// Calculate the normal for the line's width
	float dx = x2 - x1;
	float dy = y2 - y1;
	float length = std::sqrt(dx * dx + dy + dy);
	dx /= length;
	dy /= length;

	// Normal vectors for width
	const float px = -dy * halfWidth;
	const float py = dx * halfWidth;

	// Four corners of the quad
	const Vertex vertices[] =
	{
		Vertex(x1 + px, y1 + py, 0.0f, strokeR, strokeG, strokeB, strokeA),
		Vertex(x1 - px, y1 - py, 0.0f, strokeR, strokeG, strokeB, strokeA),
		Vertex(x2 + px, y2 + py, 0.0f, strokeR, strokeG, strokeB, strokeA),
		Vertex(x2 - px, y2 - py, 0.0f, strokeR, strokeG, strokeB, strokeA)
	};
	data.SetVertices(vertices, sizeof(vertices));

	// Indices for the two triangles
	uint16_t indices[] = { 0, 1, 2, 1, 3, 2 };
	data.SetIndices(indices, sizeof(indices));

	data.Normalize();
	return data;
}

//------------------------------------------------------------------------------
Rect::Rect(float x, float y, float width, float height)
	: x(x)
	, y(y)
	, width(width)
	, height(height)
{
}

//------------------------------------------------------------------------------
/*virtual*/ TessellationData Rect::Tessellate(IRenderDevice* renderDevice) const
{
	using namespace Eigen;

	TessellationData data;

	// Four corners of the rectangle
	Vector4f normalized(GetNormalizedX(x), GetNormalizedY(y), GetNormalizedX(x + width), GetNormalizedY(y + height));
	const Vertex vertices[] =
	{
		Vertex(x, y, 0.0f, fillR, fillG, fillB, fillA),						// Bottom-left
		Vertex(x + width, y, 0.0f, fillR, fillG, fillB, fillA),				// Bottom-right
		Vertex(x + width, y + height, 0.0f, fillR, fillG, fillB, fillA),	// Top-right
		Vertex(x, y + height, 0.0f, fillR, fillG, fillB, fillA),			// Top-left
	};
	data.SetVertices(vertices, sizeof(vertices));

	// Indices for the two triangles
	uint16_t indices[] = { 0, 1, 2, 0, 2, 3 };
	data.SetIndices(indices, sizeof(indices));

	data.Normalize();
	return data;
}

//------------------------------------------------------------------------------
BezierCurve::BezierCurve(float x1, float y1, float x2, float y2, float cx1, float cy1)
	: x1(x1)
	, y1(y1)
	, x2(x2)
	, y2(y2)
	, cx1(cx1)
	, cy1(cy1)
{
}

//------------------------------------------------------------------------------
/*virtual*/ TessellationData BezierCurve::Tessellate(IRenderDevice* renderDevice) const
{
	TessellationData data;

	static const int kSegments = 20;		// TODO: Allow this to be set for quality/performance tradeoff
	Vertex vertices[(kSegments + 1) * 2];	// Position + Fill color, 2 per segment: curve and baseline
	uint16_t indices[kSegments * 6];		// Two triangles per segment
	
	int32_t vertexIndex = 0;
	int32_t indexIndex = 0;

	for (int32_t i = 0; i <= kSegments; ++i)
	{
		const float t = (float)i / kSegments;
		float x = 0.0f;
		float y = 0.0f;
		ComputeXY(t, x, y);

		// Primary vertex (on the curve)
		vertices[vertexIndex++] = Vertex(x, y, 0.0f, strokeR, strokeG, strokeB, strokeA);
		
		// Baseline vertex (offset slightly downwards)
		vertices[vertexIndex++] = Vertex(x, y - strokeWidth, 0.0f, strokeR, strokeG, strokeB, strokeA);

		// Store indices
		if (i < kSegments)
		{
			// Two triangles for the segment
			uint16_t topLeft = i * 2;
			uint16_t topRight = topLeft + 2;
			uint16_t bottomLeft = topLeft + 1;
			uint16_t bottomRight = topRight + 1;

			// First triangle (top-left, bottom-left, top-right)
			indices[indexIndex++] = topLeft;
			indices[indexIndex++] = bottomLeft;
			indices[indexIndex++] = topRight;

			// Second triangle (bottom-left, bottom-right, top-right)
			indices[indexIndex++] = bottomLeft;
			indices[indexIndex++] = bottomRight;
			indices[indexIndex++] = topRight;
		}
	}

	data.SetVertices(vertices, sizeof(vertices));
	data.SetIndices(indices, sizeof(indices));

	data.Normalize();
	return data;
}

//------------------------------------------------------------------------------
void BezierCurve::ComputeXY(float t, float& x, float& y) const
{
	// Quadratic Bezier interpolation (De Casteljau's algorithm)
	x = (1.0f - t) * (1.0f - t) * x1 + 2 * (1.0f - t) * t * cx1 + t * t * x2;
	y = (1.0f - t) * (1.0f - t) * y1 + 2 * (1.0f - t) * t * cy1 + t * t * y2;
}

//------------------------------------------------------------------------------
CubicBezierCurve::CubicBezierCurve(float x1, float y1, float x2, float y2, float cx1, float cy1, float cx2, float cy2)
	: BezierCurve(x1, y1, x2, y2, cx1, cy1)
	, cx2(cx2)
	, cy2(cy2)
{
}

//------------------------------------------------------------------------------
/*virtual*/ void CubicBezierCurve::ComputeXY(float t, float& x, float& y) const
{
	// Cubic Bezier interpolation (De Casteljau's algorithm)
	x = (1.0f - t) * (1.0f - t) * (1.0f - t) * x1
	  + 3 * (1.0f - t) * (1.0f - t) * t * cx1
	  + 3 * (1.0f - t) * t * t * cx2 + t * t * t * x2;
	y = (1.0f - t) * (1.0f - t) * (1.0f - t) * y1
	  + 3 * (1.0f - t) * (1.0f - t) * t * cy1
	  + 3 * (1.0f - t) * t * t * cy2
	  + t * t * t * y2;
}

