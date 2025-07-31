#pragma once
#include "Engine.h"

#include <utility>
#include <vector>

namespace Squishies::event
{
	using Hitpoints = std::vector<std::pair<unsigned int, float>>;

	struct SplitSquishy
	{
		wf::EntityID playerId;									// player who got hit
		Hitpoints hitpoints;									// which point indices were in the radius and how far from the epicenter

		wf::Vec3 epicenter{};									// center of the explosion
		float power{};											// explosion force

		SplitSquishy(wf::EntityID playerId, const Hitpoints& hitpoints, const wf::Vec3& epicenter, float power)
			: playerId(playerId), hitpoints(hitpoints), epicenter(epicenter), power(power) {
		}
	};
}