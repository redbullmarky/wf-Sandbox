#include "SplitCombineSystem.h"
#include "Engine.h"

namespace Squishies
{
	bool SplitCombineSystem::init()
	{
		eventDispatcher->on<event::SplitSquishy>([&](event::SplitSquishy& event) {
			split(event);
			});
		return true;
	}

	void SplitCombineSystem::split(event::SplitSquishy& event)
	{
		auto& name = event.player.getComponent<wf::component::NameTag>();
		printf("GOTCHA! hitpoints on %s = %zu\n", name.name.c_str(), event.hitpoints.size());
	}
}