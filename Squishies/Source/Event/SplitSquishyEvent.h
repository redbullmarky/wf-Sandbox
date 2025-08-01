#pragma once
#include "Engine.h"

#include <utility>
#include <vector>

namespace Squishies::event
{
	using Hitpoints = std::vector<std::pair<unsigned int, float>>;

	struct SplitSquishy
	{
		wf::Entity player;										// player who got hit
		Hitpoints hitpoints;									// which point indices were in the radius and how far from the epicenter

		wf::Vec3 epicenter{};									// center of the explosion
		float power{};											// explosion force

		SplitSquishy(wf::Entity player, const Hitpoints& hitpoints, const wf::Vec3& epicenter, float power)
			: player(player), hitpoints(hitpoints), epicenter(epicenter), power(power) {
		}
	};
}