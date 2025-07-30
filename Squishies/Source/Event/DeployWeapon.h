#pragma once
#include "Engine.h"

namespace Squishies::event
{
	struct DeployWeapon
	{
		wf::EntityID playerId;
		int weaponId;

		wf::Vec3 position;
		wf::Vec3 target;
		float power;

		DeployWeapon(wf::EntityID playerId, int weaponId, const wf::Vec3& pos, const wf::Vec3& target, float power)
			: playerId(playerId), weaponId(weaponId), position(pos), target(target), power(power) {
		}
	};
}