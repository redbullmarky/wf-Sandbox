#include "Scene.h"

#include "Component/Transform.h"
#include "System/CoreSystem.h"

namespace Sandbox
{
	bool Scene::init()
	{
		addSystem<CoreSystem>();
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

	wf::Entity Scene::createObject(const wf::Vec3& position)
	{
		auto ob = createEmptyObject();
		ob.addComponent<Component::Transform>(position);
		return ob;
	}

	wf::Entity Scene::createCamera(const wf::Vec3& position, const wf::Vec3& target, bool ortho, float fovOrWidth)
	{
		auto object = createObject(position);

		Component::Camera cam;

		if (ortho) {
			cam = Component::Camera::createOrthographic(
				position,
				target,
				fovOrWidth > 0.f ? fovOrWidth : 10.f
			);
		}
		else {
			cam = Component::Camera::createPerspective(
				position,
				target,
				fovOrWidth > 0.f ? fovOrWidth : 60.f
			);
		}

		auto& camera = object.addComponent<Component::Camera>(cam);
		if (!config.currentCamera) {
			config.currentCamera = &camera;
		}

		return object;
	}

	Component::Camera* Scene::getCurrentCamera()
	{
		return config.currentCamera;
	}
}