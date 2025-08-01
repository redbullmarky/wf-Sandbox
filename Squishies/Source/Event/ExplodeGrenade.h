#pragma once
#include "Engine.h"

namespace Squishies::event
{
	struct ExplodeGrenade
	{
		wf::Entity id;
		wf::Entity player;
		wf::Vec3 position;

		ExplodeGrenade(wf::Entity id, wf::Entity player, const wf::Vec3& position)
			: id(id), player(player), position(position) {
		}
	};
}