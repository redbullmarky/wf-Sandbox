#include "BaseScene.h"

#include "System/CoreSystem.h"

namespace Sandbox
{
	bool BaseScene::init()
	{
		addSystem<CoreSystem>();

		return wf::Scene::init();
	}
}