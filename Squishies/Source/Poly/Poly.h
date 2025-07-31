#pragma once
#include "Engine.h"

#include <vector>

namespace Squishies
{
	struct Poly
	{
		std::vector<wf::Vec2> points;										// points that make up the edge
		wf::Vec2 origin{};													// origin for transforms

		Poly() = default;
		Poly(std::vector<wf::Vec2> points, wf::Vec2 origin = {});

		/**
		 * @brief Return the center of the poly based on its verts
		 */
		wf::Vec2 getCenter() const;

		/**
		 * @brief Translate the points from the origin.
		 *
		 * As the transform is/can be 3D, this would also skew the points
		 */
		Poly& transform(const wf::Transform& transform);

		/**
		 * @brief Move the points
		 */
		Poly& translate(const wf::Vec2& offset);

		/**
		 * @brief Angle to rotate, CCW in degrees around the origin
		 */
		Poly& rotate(float angleDegrees);

		/**
		 * @brief Scale the points from the origin
		 */
		Poly& scale(float scale);
	};
}