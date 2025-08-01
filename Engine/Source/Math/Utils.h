#pragma once
#include "Math/Math.h"

namespace wf
{
	/**
	 * @brief 2D "cross product"
	 */
	float cross2D(const Vec2& vec, const Vec2& other);

	/**
	 * @brief Get perpendicular vector clockwise
	 */
	Vec2 perpCW(const Vec2& vec);

	/**
	 * @brief Get perpendicular vector counterclockwise
	 */
	Vec2 perpCCW(const Vec2& vec);

	/**
	 * @brief Calculate spring force given two points with their respective position & velocity
	 */
	Vec2 getSpringForce(const Vec2& p1, const Vec2& v1, const Vec2& p2, const Vec2& v2, float k, float damping, float rest);

	/**
	 * @brief 3D version of the above
	 */
	Vec3 getSpringForce(const Vec3& p1, const Vec3& v1, const Vec3& p2, const Vec3& v2, float k, float damping, float rest);
}