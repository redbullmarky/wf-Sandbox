#pragma once
#include "Engine.h"

#include <memory>
#include <vector>

namespace Squishies
{
	/**
	 * @brief Given a set of vertex positions, will transform it given derived position/rotation.
	 *
	 * Used by shape matching
	 */
	std::vector<wf::Vec3> transformPoints(
		const std::vector<wf::Vec3>& originalPoints,
		const wf::Vec3& derivedPosition,
		const wf::Quat& derivedRotation
	);

	/**
	 * @brief Generate a simple circle of points
	 * @param radius
	 * @param segments
	 */
	std::vector<wf::Vec3> createCircleShape(float radius, size_t segments);

	/**
	 * @brief Converts a previously generated list of circle points into a mesh with center point
	 * @param edgePoints
	 * @return
	 */
	std::shared_ptr<wf::Mesh> createCircleMeshFromPoints(const std::vector<wf::Vec3>& edgePoints);
}