#include "PolyFactory.h"
#include "Engine.h"

#include <vector>

namespace Squishies
{
	Poly PolyFactory::createRect(float width, float height, int extraPointsX, int extraPointsY)
	{
		Poly poly;

		int stepsX = extraPointsX + 1;
		int stepsY = extraPointsY + 1;

		float halfW = width * 0.5f;
		float halfH = height * 0.5f;

		// Left edge (top to bottom)
		for (int i = 0; i <= stepsY; i++) {
			if (i == 0) poly.primaryPoints.push_back(poly.points.size());
			poly.points.emplace_back(-halfW, halfH - (height * i) / stepsY);
		}

		// Bottom edge (left to right)
		for (int i = 1; i <= stepsX; i++) {
			if (i == stepsX) poly.primaryPoints.push_back(poly.points.size());
			poly.points.emplace_back(-halfW + (width * i) / stepsX, -halfH);
		}

		// Right edge (bottom to top)
		for (int i = 1; i <= stepsY; i++) {
			if (i == stepsY) poly.primaryPoints.push_back(poly.points.size());
			poly.points.emplace_back(halfW, -halfH + (height * i) / stepsY);
		}

		// Top edge (right to left)
		for (int i = 1; i < stepsX; i++) {
			if (i == stepsX - 1) poly.primaryPoints.push_back(poly.points.size());
			poly.points.emplace_back(halfW - (width * i) / stepsX, halfH);
		}

		return poly;
	}


	Poly PolyFactory::createSquare(float size, int extraPoints)
	{
		return createRect(size, size, extraPoints, extraPoints);
	}

	Poly PolyFactory::createCircle(float radius, int segments)
	{
		return createEllipse(radius, radius, segments);
	}

	Poly PolyFactory::createEllipse(float radiusX, float radiusY, int segments)
	{
		std::vector<wf::Vec2> points;
		points.resize(segments);

		for (size_t i = 0; i < segments; i++) {
			float angle = (2.f * PI * i) / segments;
			float x = radiusX * cos(angle);
			float y = radiusY * sin(angle);

			points[i] = { x, y };
		}

		return Poly(points);
	}

	Poly PolyFactory::createGear(float radius, int teeth, float toothDepth)
	{
		int segments = teeth * 4;
		float angleStep = (2.f * PI) / segments;

		std::vector<wf::Vec2> points;
		points.reserve(segments);

		for (int i = segments - 1; i >= 0; i--) {
			bool outside = (i % 4) < 2;
			float baseAngle = angleStep * i;
			float r = outside ? radius : radius - toothDepth;

			float adjustedAngle = baseAngle;
			if (outside) {
				adjustedAngle += (i % 4 == 0) ? -angleStep * 0.5f : angleStep * 0.5f;
			}

			float x = std::cos(adjustedAngle) * r;
			float y = std::sin(adjustedAngle) * r;

			points.emplace_back(x, y);
		}

		std::reverse(points.begin(), points.end());

		return Poly(points);
	}
}