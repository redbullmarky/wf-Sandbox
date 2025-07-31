#include "SquishyFactory.h"
#include "Engine.h"

#include "PolyFactory.h"

#include <vector>

namespace Squishies
{
	Squishy SquishyFactory::createRect(float width, float height)
	{
		int extraX = static_cast<int>(floor(width - 1));
		int extraY = static_cast<int>(floor(height - 1));

		return PolyFactory::createRect(width, height, extraX, extraY);
	}

	Squishy SquishyFactory::createSquare(float size)
	{
		return createRect(size, size);
	}

	Squishy SquishyFactory::createCircle(float radius, int segments, int strength)
	{
		Squishy s(PolyFactory::createCircle(radius, segments));

		if (strength > 0) {

			// build out the joints base on strenght setting
			for (size_t i = 0; i < s.poly.points.size(); i++) {
				for (size_t step = 1; step <= strength; step++) {
					if (step >= s.poly.points.size()) break; // don't loop entire shape
					size_t j = (i + step) % s.poly.points.size();
					float dist = glm::distance(s.poly.points[i], s.poly.points[j]);
					s.joints.emplace_back(i, j, dist);
				}
			}
		}

		return s;
	}

	Squishy SquishyFactory::createGear(float radius, int teeth, float toothDepth)
	{
		return PolyFactory::createGear(radius, teeth, toothDepth);
	}
}