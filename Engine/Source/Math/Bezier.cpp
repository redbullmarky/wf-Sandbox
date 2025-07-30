#include "pch.h"
#include "Math/Bezier.h"
#include "Math/Math.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace wf
{
	Vec3 Bezier::getPosition(float t) const
	{
		Vec3 a = glm::mix(controlPoints[0], controlPoints[1], t);
		Vec3 b = glm::mix(controlPoints[1], controlPoints[2], t);
		Vec3 c = glm::mix(controlPoints[2], controlPoints[3], t);

		Vec3 d = glm::mix(a, b, t);
		Vec3 e = glm::mix(b, c, t);

		return glm::mix(d, e, t);
	}

	Vec3 Bezier::getTangent(float t) const
	{
		Vec3 a = glm::mix(controlPoints[0], controlPoints[1], t);
		Vec3 b = glm::mix(controlPoints[1], controlPoints[2], t);
		Vec3 c = glm::mix(controlPoints[2], controlPoints[3], t);

		Vec3 d = glm::mix(a, b, t);
		Vec3 e = glm::mix(b, c, t);

		return glm::normalize(e - d);
	}

	Quat Bezier::getOrientation(float t) const
	{
		Vec3 tangent = getTangent(t);
		Vec3 up = Vec3(0, 1, 0);
		return glm::quatLookAt(glm::normalize(tangent), up);
	}

	Mat4 Bezier::getTransform(float t) const
	{
		auto pos = getPosition(t);
		auto rot = getOrientation(t);

		Mat4 transform(1.0f);
		transform.matrix = glm::translate(glm::mat4(1.0f), pos) * glm::toMat4(rot);
		return transform;
	}
}
