#include "BaseScene.h"

#include "System/CameraSystem.h"
#include "System/CoreSystem.h"

namespace Sandbox
{
	bool BaseScene::init()
	{
		addSystem<CoreSystem>();
		addSystem<CameraSystem>();

		return wf::Scene::init();
	}
}