#include "CameraSystem.h"
#include "Engine.h"

namespace Sandbox
{
	bool CameraSystem::init()
	{
		return true;
	}

	void CameraSystem::update(float dt)
	{
		// regular freecam
		if (wf::isKeyHeld(wf::KEY_SHIFT_LEFT)) {
			scene->getCurrentCamera()->updateFree(dt);
		}
	}
}