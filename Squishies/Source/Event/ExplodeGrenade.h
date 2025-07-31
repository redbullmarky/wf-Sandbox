#pragma once
#include "Engine.h"

namespace Squishies::event
{
	struct ExplodeGrenade
	{
		wf::EntityID id;
		wf::EntityID playerId;
		wf::Vec3 position;

		ExplodeGrenade(wf::EntityID id, wf::EntityID playerId, const wf::Vec3& position)
			: id(id), playerId(playerId), position(position) {
		}
	};
}