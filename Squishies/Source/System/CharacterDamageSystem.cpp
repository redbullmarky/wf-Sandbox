#include "CharacterDamageSystem.h"
#include "Engine.h"

namespace Squishies
{
	bool CharacterDamageSystem::init()
	{
		eventDispatcher->on<event::SplitSquishy>([&](event::SplitSquishy& event) {
			split(event);
			});
		return true;
	}

	void CharacterDamageSystem::split(event::SplitSquishy& event)
	{
		auto& name = event.player.getComponent<wf::component::NameTag>();
		printf("GOTCHA! hitpoints on %s = %zu\n", name.name.c_str(), event.hitpoints.size());

		// @todo
		// 1. amend the original player to have less verts
		// 2. create a copy
		// 3. send them off away from the blast radius and/or using the average position of the 


	}
}