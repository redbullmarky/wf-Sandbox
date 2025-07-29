#include "pch.h"
#include "Scene.h"

#include "Component/Camera.h"
#include "Component/Transform.h"
#include "Entity.h"

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

	void Scene::setBackgroundColour(const Colour& colour)
	{
		config.backgroundColour = colour;
	}

	Colour Scene::getBackgroundColour() const
	{
		return config.backgroundColour;
	}

	Entity Scene::createEmptyObject()
	{
		return entityManager.create();
	}

	[[nodiscard]] Entity Scene::createObject(const wf::Vec3& position)
	{
		auto ob = createEmptyObject();
		ob.addComponent<component::Transform>(position);
		return ob;
	}

	Entity Scene::createCamera(const Vec3& position, const Vec3& target, bool ortho, float fovOrWidth)
	{
		auto object = createObject(position);

		component::Camera cam;

		if (ortho) {
			cam = component::Camera::createOrthographic(
				position,
				target,
				fovOrWidth > 0.f ? fovOrWidth : 10.f
			);
		}
		else {
			cam = component::Camera::createPerspective(
				position,
				target,
				fovOrWidth > 0.f ? fovOrWidth : 60.f
			);
		}

		auto& camera = object.addComponent<component::Camera>(cam);
		if (!currentCamera) {
			currentCamera = &camera;
		}

		return object;
	}

	component::Camera* Scene::getCurrentCamera()
	{
		return currentCamera;
	}

	EntityManager* Scene::getEntityManager()
	{
		return &entityManager;
	}
}