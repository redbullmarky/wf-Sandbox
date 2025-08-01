#include "pch.h"
#include "Scene.h"

#include "Component/Camera.h"
#include "Component/Light.h"
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
		ob.addComponent<TransformComponent>(position);
		return ob;
	}

	CameraComponent* Scene::createCamera(const Vec3& position, const Vec3& target, bool ortho, float fovOrWidth)
	{
		auto object = createObject(position);

		CameraComponent cam;

		if (ortho) {
			cam = CameraComponent::createOrthographic(
				position,
				target,
				fovOrWidth > 0.f ? fovOrWidth : 10.f
			);
		}
		else {
			cam = CameraComponent::createPerspective(
				position,
				target,
				fovOrWidth > 0.f ? fovOrWidth : 60.f
			);
		}

		auto& camera = object.addComponent<CameraComponent>(cam);
		if (!currentCamera) {
			currentCamera = &camera;
		}

		return &camera;
	}

	CameraComponent* Scene::getCurrentCamera()
	{
		return currentCamera;
	}

	LightComponent* Scene::createLight(const Vec3& position, const Vec3& target)
	{
		auto object = createObject();

		LightComponent light(position, target);

		auto& lightCmp = object.addComponent<LightComponent>(light);
		if (!currentLight) {
			currentLight = &lightCmp;
		}

		return &lightCmp;
	}

	LightComponent* Scene::getCurrentLight()
	{
		return currentLight;
	}

	EntityManager* Scene::getEntityManager()
	{
		return &entityManager;
	}

	EventDispatcher* Scene::getEventDispatcher()
	{
		return &eventDispatcher;
	}
}