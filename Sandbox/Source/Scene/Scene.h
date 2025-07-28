#pragma once
#include "Engine.h"

#include "Component/Camera.h"

#include <memory>

namespace Sandbox
{
	struct SceneConfig
	{
		wf::Colour backgroundColour = wf::CUTTINGMAT;
		Component::Camera* currentCamera{ nullptr };
	};

	class Scene
	{
	public:
		Scene() = default;
		virtual ~Scene() = default;

		virtual bool init();
		virtual void shutdown();

		virtual void update(float dt);
		virtual void fixedUpdate(float dt);
		virtual void render(float dt);

		/**
		 * @brief Set the solid colour used to clear the background before render
		 */
		void setBackgroundColour(const wf::Colour& colour);

		/**
		 * @brief Create a plain, undecorated entity
		 */
		wf::Entity createEmptyObject();

		/**
		 * @brief Create a "game object" with a transform component and any other essentials for visual items
		 */
		wf::Entity createObject(const wf::Vec3& position = { 0.f, 0.f, 0.f });

		/**
		 * @brief Create a perspective (default) or ortho camera. If it's the first camera, it'll also be used as the default
		 */
		wf::Entity createCamera(const wf::Vec3& position, const wf::Vec3& target, bool ortho = false, float fovOrWidth = -1.f);

		Component::Camera* getCurrentCamera();

		/**
		 * @brief Register and initialise a system
		 */
		template<typename T, typename... Args>
		T& addSystem(Args&&... args)
		{
			static_assert(std::is_base_of<wf::ISystem, T>::value, "T must derive from wf::ISystem");
			m_systems.push_back(std::make_unique<T>(std::forward<Args>(args)...));
			if (!(T*)(m_systems.back())->init()) {
				throw std::runtime_error("System init() returned false: " + std::string(typeid(T).name()));
			}
			return dynamic_cast<T&>(*m_systems.back());
		}

	protected:
		wf::EntityManager entityManager;
		SceneConfig config;

	private:
		std::vector<std::unique_ptr<wf::ISystem>> m_systems;
	};
}