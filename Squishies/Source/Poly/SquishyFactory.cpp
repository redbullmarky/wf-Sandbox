#include "SquishyFactory.h"
#include "Engine.h"

#include "PolyFactory.h"

#include <vector>

namespace Squishies
{
	Squishy SquishyFactory::createRect(float width, float height)
	{
		Squishy s(PolyFactory::createRect(width, height));

		//// support
		//int widthSupport = static_cast<int>(ceil(width / height));
		//int heightSupport = static_cast<int>(ceil(height / width));

		//float widthSeg = width / widthSupport;
		//float heightSeg = height / heightSupport;

		//size_t vertcount = (widthSupport * 2) + ((heightSupport - 1) * 2);

		//std::vector<Vector2> verts(vertcount);

		//// todo: vertical supports
		//int c = 0;
		//for (int x = 0; x < widthSupport; x++) {
		//	verts[vertcount - x - 1] = { 0.f + (x * widthSeg), 0.f };
		//	verts[x] = { 0.f + (x * widthSeg), height };
		//	c += 2;
		//}

		//return { verts };

		return s;
	}

	Squishy SquishyFactory::createSquare(float size)
	{
		return createRect(size, size);
	}

	Squishy SquishyFactory::createCircle(float radius, int segments)
	{
		Squishy s(PolyFactory::createCircle(radius, segments));

		// next build out the joints
		int strength = 3;

		for (size_t i = 0; i < s.poly.points.size(); i++) {
			if (strength <= 0) break;

			for (size_t step = 1; step <= strength; step++) {
				if (step >= s.poly.points.size()) break; // don't loop entire shape
				size_t j = (i + step) % s.poly.points.size();
				float dist = glm::distance(s.poly.points[i], s.poly.points[j]);
				s.joints.emplace_back(i, j, dist);
			}
		}

		return s;
	}

	Squishy SquishyFactory::createGear(float radius, int teeth, float toothDepth)
	{
		Squishy s(PolyFactory::createGear(radius, teeth, toothDepth));

		// @todo joints

		return s;
	}
}