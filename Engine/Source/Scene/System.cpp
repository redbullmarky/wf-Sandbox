#include "pch.h"
#include "System.h"

#include "EntityManager.h"
#include "Scene.h"

namespace wf
{
	ISystem::ISystem(Scene* scene) : scene(scene), entityManager(scene->getEntityManager())
	{
	}
}