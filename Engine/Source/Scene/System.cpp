#include "pch.h"
#include "System.h"

#include "Core/EntityManager.h"
#include "Scene.h"

namespace wf
{
	ISystem::ISystem(Scene* scene)
		: scene(scene),
		entityManager(scene->getEntityManager()),
		eventDispatcher(scene->getEventDispatcher())
	{
	}
}