#include "SquishyFactory.h"
#include "Engine.h"

#include "PolyFactory.h"

#include <vector>

namespace Squishies
{
	Squishy SquishyFactory::createRect(float width, float height, wf::Colour colour)
	{
		int extraX = static_cast<int>(floor(width - 1));
		int extraY = static_cast<int>(floor(height - 1));

		Squishy s(PolyFactory::createRect(width, height, extraX, extraY));
		s.colour = colour;

		return s;
	}

	Squishy SquishyFactory::createSquare(float size, wf::Colour colour)
	{
		return createRect(size, size, colour);
	}

	Squishy SquishyFactory::createCircle(float radius, int segments, int strength, wf::Colour colour)
	{
		Squishy s(PolyFactory::createCircle(radius, segments));
		s.colour = colour;

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

	// kept separate from circle for now as we may need specific strength requirements
	Squishy SquishyFactory::createEllipse(float radiusX, float radiusY, int segments, int strength, wf::Colour colour)
	{
		Squishy s(PolyFactory::createEllipse(radiusX, radiusY, segments));
		s.colour = colour;

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

	Squishy SquishyFactory::createGear(float radius, int teeth, float toothDepth, wf::Colour colour)
	{
		Squishy s(PolyFactory::createGear(radius, teeth, toothDepth));
		s.colour = colour;

		return s;
	}
}