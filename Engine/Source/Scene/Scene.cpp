#include "pch.h"
#include "Scene.h"

namespace wf
{
	bool Scene::init()
	{
		for (auto& system : m_systems) {
			if (!system->init()) {
				return false;
			}
		}
		return true;
	}

	void Scene::shutdown()
	{
		teardown();

		for (auto it = m_systems.rbegin(); it != m_systems.rend(); ++it) {
			(*it)->shutdown();
		}
	}

	void Scene::setup()
	{
		for (auto& system : m_systems) {
			system->setup();
		}
	}

	void Scene::teardown()
	{
		for (auto& system : m_systems) {
			system->teardown();
		}
		entityManager.clear();
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

	EntityManager* Scene::getEntityManager()
	{
		return &entityManager;
	}
}