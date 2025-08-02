#pragma once
#include "EntityManager.h"

namespace wf
{
	using ResourceID = entt::entity;

	class ResourceManager : public EntityManager
	{
	public:
		using EntityManager::EntityManager;

		void shutdown();
	};
}