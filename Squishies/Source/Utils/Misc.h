#pragma once
#include "Engine.h"

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
}