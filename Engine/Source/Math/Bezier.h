#pragma once
#include "Geometry/Geometry.h"

#include <vector>

namespace wf
{
	struct Bezier
	{
		std::vector<Vec3> controlPoints;

		Bezier(std::vector<Vec3> controlPoints) : controlPoints(controlPoints) {}

		Vec3 getPosition(float t) const;
		Vec3 getTangent(float t) const;
		Quat getOrientation(float t) const;
		Mat4 getTransform(float t) const;
	};
}
