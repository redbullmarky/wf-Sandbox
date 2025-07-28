#include "CoreSystem.h"

#include "Component/Geometry.h"

namespace Sandbox
{
	CoreSystem::CoreSystem(wf::Scene* scene) : wf::ISystem(scene)
	{
	}

	bool CoreSystem::init()
	{
		return true;
	}

	void CoreSystem::update(float dt)
	{

	}

	void CoreSystem::shutdown()
	{
	}
}