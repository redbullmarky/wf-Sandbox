#pragma once
#include "EntityManager.h"
#include "Math/Math.h"

namespace wf
{
	class Scene
	{
	public:
		Scene() = default;
		~Scene() = default;

		Entity createObject();
		Entity createObject(const Vec3& position);

	public:
		EntityManager entityManager;
	};
}