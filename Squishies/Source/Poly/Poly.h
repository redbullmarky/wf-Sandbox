#pragma once
#include "Engine.h"

#include <vector>

namespace Squishies
{
	struct Poly
	{
		std::vector<wf::Vec2> points;										// points that make up the edge
		std::vector<size_t> primaryPoints;									// those deemed to be key to the shape rather than supporting

		Poly() = default;
		Poly(std::vector<wf::Vec2> points);

		/**
		 * @brief Return the center of the poly based on its verts
		 */
		wf::Vec2 getCenter() const;

		/**
		 * @brief Translate the points
		 *
		 * As the transform is/can be 3D, this would also skew the points. @todo
		 */
		Poly& transform(const wf::Transform& transform);

		/**
		 * @brief Create a new instance with transformed points
		 */
		Poly getTransformed(const wf::Transform& transform) const;

		/**
		 * @brief Move the points
		 */
		Poly& translate(const wf::Vec2& offset);

		/**
		 * @brief Angle to rotate, CCW in degrees
		 */
		Poly& rotate(float angleDegrees);

		/**
		 * @brief Scale the points
		 */
		Poly& scale(float scale);
	};
}