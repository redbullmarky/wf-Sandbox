#include "pch.h"
#include "Scene.h"

namespace wf
{
	bool Scene::init()
	{
		// unlike the other methods that map to a system equivalent, the init() of each system is done when it's first created
		return true;
	}

	void Scene::shutdown()
	{
		for (auto it = m_systems.rbegin(); it != m_systems.rend(); ++it) {
			(*it)->shutdown();
		}
	}

	void Scene::update(float dt)
	{
		for (auto& system : m_systems) {
			system->update(dt);
		}
	}

	void Scene::fixedUpdate(float dt)
	{
		for (auto& system : m_systems) {
			system->fixedUpdate(dt);
		}
	}

	void Scene::render(float dt)
	{
		for (auto& system : m_systems) {
			system->render(dt);
		}
	}

	void Scene::setBackgroundColour(const wf::Colour& colour)
	{
		config.backgroundColour = colour;
	}

	wf::Entity Scene::createEmptyObject()
	{
		return entityManager.create();
	}
}