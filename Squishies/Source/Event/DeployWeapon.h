#pragma once
#include "Engine.h"

namespace Squishies::event
{
	struct DeployWeapon
	{
		wf::Entity player;
		int weaponId;

		wf::Vec3 position;
		wf::Vec3 target;
		float power;

		DeployWeapon(wf::Entity player, int weaponId, const wf::Vec3& pos, const wf::Vec3& target, float power)
			: player(player), weaponId(weaponId), position(pos), target(target), power(power) {
		}
	};
}