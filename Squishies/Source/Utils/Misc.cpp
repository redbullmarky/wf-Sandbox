#include "Misc.h"

namespace Squishies
{
	std::vector<wf::Vec3> Squishies::transformPoints(
		const std::vector<wf::Vec3>& originalPoints,
		const wf::Vec3& derivedPosition,
		const wf::Quat& derivedRotation
	) {
		std::vector<wf::Vec3> transformed;
		transformed.reserve(originalPoints.size());

		for (const auto& p : originalPoints)
			transformed.push_back(derivedPosition + derivedRotation * p);

		return transformed;
	}
}