#pragma once
namespace wf
{
	class EntityManager;
	class EventDispatcher;
	class Scene;

	/**
	 * @brief Base system for operating on the scene
	 *
	 * Refer to Scene itself for the methods, as they're named the name as the scene, with the only exception being 'init()'
	 */
	class ISystem
	{
	public:
		ISystem(Scene* scene);
		virtual ~ISystem() = default;

		virtual bool init() = 0;
		virtual void shutdown() {}

		virtual void setup() {}
		virtual void teardown() {}

		virtual void update(float dt) {}
		virtual void fixedUpdate(float dt) {}
		virtual void render(float dt) {}

	protected:
		Scene* scene;
		EntityManager* entityManager;
		EventDispatcher* eventDispatcher;
	};
}