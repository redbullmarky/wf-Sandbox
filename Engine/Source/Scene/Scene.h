#pragma once
#include "Core/EntityManager.h"
#include "Core/EventDispatcher.h"
#include "Geometry/Geometry.h"
#include "Misc/Colour.h"
#include "System.h"

#include <memory>

namespace wf
{
	struct CameraComponent;
	struct LightComponent;

	struct SceneConfig
	{
		Colour backgroundColour = CUTTINGMAT;
	};

	class Scene
	{
	public:
		Scene() = default;
		virtual ~Scene() = default;

		/**
		 * @brief Invoked when the scene is physically created
		 *
		 * Typically this would be for setup of global stuff as well as event registration. Also initialises all of the systems. Essentially, a constructor...
		 */
		virtual bool init();

		/**
		 * @brief Invoked when the scene is destroyed. Essentially, a destructor of sorts
		 */
		virtual void shutdown();

		/**
		 * @brief Invoked when the scene is started
		 *
		 * This generally would be where the scene is populated, now that the events etc are all ready in all active systems following the init()
		 */
		virtual void setup() = 0;

		/**
		 * @brief Invoked when the scene is left
		 */
		virtual void teardown();

		/**
		 * @brief Regular update
		 */
		virtual void update(float dt);

		/**
		 * @brief Update with fixed timestep.
		 *
		 * Typically for physics stuff; This can run multiple times per frame
		 */
		virtual void fixedUpdate(float dt);

		/**
		 * @brief Render the scene
		 */
		virtual void render(float dt);

		/**
		 * @brief A place for ImGui to do its thing
		 */
		virtual void renderGui(float dt) {}

		/**
		 * @brief Set the solid colour used to clear the background before render
		 */
		void setBackgroundColour(const Colour& colour);

		/**
		 * Fetch the background colour
		 */
		Colour getBackgroundColour() const;

		/**
		 * @brief Create a plain, undecorated entity
		 */
		Entity createEmptyObject();

		/**
		 * @brief Create a "game object" with a transform component and any other essentials for visual items
		 */
		Entity createObject(const Vec3& position = { 0.f, 0.f, 0.f });

		/**
		 * @brief Create a perspective (default) or ortho camera. If it's the first camera, it'll also be used as the default
		 */
		CameraComponent* createCamera(const Vec3& position, const Vec3& target, bool ortho = false, float fovOrWidth = -1.f);

		/**
		 * @brief Fetch the camera currently in use
		 */
		CameraComponent* getCurrentCamera();

		/**
		 * @brief Create a simple light @todo only one for now for testing shadows, we'll need more.
		 */
		LightComponent* createLight(const Vec3& position, const Vec3& target);

		/**
		 * @brief Fetch the main light. @todo see createLight()
		 */
		LightComponent* getCurrentLight();

		/**
		 * @brief Return the entity manager attached to this scene
		 */
		EntityManager* getEntityManager();

		/**
		 * @brief Return the event dispatcher attached to this scene
		 */
		EventDispatcher* getEventDispatcher();

		/**
		 * @brief Register and initialise a system
		 */
		template<typename T, typename... Args>
		T& addSystem(Args&&... args)
		{
			static_assert(std::is_base_of<ISystem, T>::value, "T must derive from ISystem");
			m_systems.push_back(std::make_unique<T>(this, std::forward<Args>(args)...));
			return dynamic_cast<T&>(*m_systems.back());
		}

	protected:
		EntityManager entityManager;
		EventDispatcher eventDispatcher;
		SceneConfig config;
		CameraComponent* currentCamera{ nullptr };
		LightComponent* currentLight{ nullptr };

	private:
		std::vector<std::unique_ptr<ISystem>> m_systems;
	};
}