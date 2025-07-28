#pragma once
#include "Common.h"
#include "EntityManager.h"
#include "Misc/Colour.h"
#include "System.h"

#include <memory>

namespace wf
{
	struct SceneConfig
	{
		Colour backgroundColour = CUTTINGMAT;
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
		void setBackgroundColour(const Colour& colour);

		/**
		 * @brief Create a plain, undecorated entity
		 */
		Entity createEmptyObject();

		/**
		 * @brief Register and initialise a system
		 */
		template<typename T, typename... Args>
		T& addSystem(Args&&... args)
		{
			static_assert(std::is_base_of<ISystem, T>::value, "T must derive from ISystem");
			m_systems.push_back(std::make_unique<T>(this, std::forward<Args>(args)...));
			if (!(T*)(m_systems.back())->init()) {
				throw std::runtime_error("System init() returned false: " + std::string(typeid(T).name()));
			}
			return dynamic_cast<T&>(*m_systems.back());
		}

	protected:
		EntityManager entityManager;
		SceneConfig config;

	private:
		std::vector<std::unique_ptr<ISystem>> m_systems;
	};
}