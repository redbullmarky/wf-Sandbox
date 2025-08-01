#include "pch.h"
#include "Utils.h"

namespace wf
{
	float cross2D(const Vec2& vec, const Vec2& other)
	{
		return (vec.x * other.y) - (vec.y * other.x);
	}

	Vec2 perpCW(const Vec2& vec)
	{
		return { vec.y, -vec.x };
	}

	Vec2 perpCCW(const Vec2& vec)
	{
		return { -vec.y, vec.x };
	}

	Vec2 getSpringForce(const Vec2& p1, const Vec2& v1, const Vec2& p2, const Vec2& v2, float k, float damping, float rest)
	{
		auto ret = getSpringForce(
			Vec3{ p1.x, p1.y, 0.f },
			Vec3{ v1.x, v1.y, 0.f },
			Vec3{ p2.x, p2.y, 0.f },
			Vec3{ v2.x, v2.y, 0.f },
			k,
			damping,
			rest
		);
		return { ret.x, ret.y };
	}

	Vec3 getSpringForce(const Vec3& p1, const Vec3& v1, const Vec3& p2, const Vec3& v2, float k, float damping, float rest)
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