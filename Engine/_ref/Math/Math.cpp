#include "pch.h"
#include "Math/Math.h"

namespace wf
{
	Mat4& Mat4::translate(const Vec3& pt)
	{
		matrix = glm::translate(matrix, pt);
		return *this;
	}

	Mat4& Mat4::scale(const Vec3& dims)
	{
		matrix = glm::scale(matrix, dims);
		return *this;
	}

	Mat4& Mat4::rotate(float angleRadians, const Vec3& axis)
	{
		matrix = glm::rotate(matrix, angleRadians, axis);
		return *this;
	}

	Mat4 Mat4::lookAt(Vec3 pos, Vec3 target, Vec3 up)
	{
		return Mat4{ glm::lookAt(pos, target, up) };
	}

	Mat4 Mat4::operator*(const Mat4& rhs) const
	{
		return Mat4{ matrix * rhs.matrix };
	}

	Vec3 getSpringForce(Vec3 p1, Vec3 v1, Vec3 p2, Vec3 v2, float k, float damping, float rest)
	{
		Vec3 distanceV = p1 - p2;
		float dist = glm::length(distanceV);

		if (dist < .01f) {
			return Vec3{};
		}

		distanceV = distanceV / dist;

		float displacement = rest - dist;
		float totalRelVel = glm::dot(v1 - v2, distanceV);

		return distanceV * ((displacement * k) - (totalRelVel * damping));
	}
}