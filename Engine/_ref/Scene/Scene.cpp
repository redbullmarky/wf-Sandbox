#include "pch.h"
#include "Scene.h"
#include "Component/Transform.h"

namespace wf
{
	Entity Scene::createObject()
	{
		return createObject(Vec3{});
	}

	Entity Scene::createObject(const Vec3& position)
	{
		auto entity = entityManager.create();
		entity.addComponent<scene::Transform>(position);
		return entity;
	}
}