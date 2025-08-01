#include "CharacterDamageSystem.h"
#include "Engine.h"

#include "Component/SoftBody.h"
#include "Poly/Squishy.h"

#include <vector>

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
		auto& name = event.player.getComponent<wf::NameTagComponent>();
		printf("GOTCHA! hitpoints on %s = %zu\n", name.name.c_str(), event.hitpoints.size());

		// @todo
		// 1. amend the original player to have less verts
		// 2. create a copy
		// 3. send them off away from the blast radius and/or using the average position of the 

		//auto& softbody = event.player.getComponent<Component::SoftBody>();

		//// for now, just an even split
		//int numPoints = (int)softbody.points.size();
		//int spawnPoints = (int)floor(numPoints / 2);
		//int remainingPoints = numPoints - spawnPoints;

		//spawnPoints = std::max(spawnPoints, 5);

		//std::vector<Component::PointMass> newPoints;
		//std::vector<Component::PointMass> spawnPoints;

		//printf("split is %d %d\n", remainingPoints, spawnPoints);

		//Squishy spawn;
		//spawn.po
	}
}