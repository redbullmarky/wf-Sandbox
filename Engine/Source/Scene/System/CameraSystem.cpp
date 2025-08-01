#include "pch.h"
#include "CameraSystem.h"

#include "Core/Core.h"
#include "Scene/Component/CameraComponent.h"
#include "Scene/Scene.h"

namespace wf::system
{
	bool CameraSystem::init()
	{
		return true;
	}

	void CameraSystem::update(float dt)
	{
		// regular freecam
		if (isKeyHeld(KEY_SHIFT_LEFT)) {
			scene->getCurrentCamera()->updateFree(dt);
		}
	}
}