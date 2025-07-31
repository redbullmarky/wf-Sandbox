#pragma once
#include "Common.h"

#include <entt/entt.hpp>
#include <string>

namespace wf
{
	struct Entity
	{
		Entity(entt::registry& registry, EntityID handle, const std::string& name = "") : registry(registry), handle(handle), name(name)
		{
		}

		bool isValid() const
		{
			return registry.valid(handle);
		}

		// For empty structs
		template<typename T, typename std::enable_if_t<std::is_empty_v<T>, int> = 0>
		void addComponent()
		{
			assert(!hasComponent<T>());
			registry.emplace<T>(handle);
		}

		template<typename T, typename... Args>
		std::enable_if_t<std::is_constructible_v<T, Args&&...>, T&>
			addComponent(Args&&... args)
		{
			assert(!hasComponent<T>());
			T& component = registry.emplace<T>(handle, std::forward<Args>(args)...);

			return component;
		}

		template<typename T>
		std::enable_if_t<std::is_aggregate_v<T>, T&>
			addComponent(T&& aggregate)
		{
			assert(!hasComponent<std::decay_t<T>>());
			T& component = registry.emplace<std::decay_t<T>>(handle, std::forward<T>(aggregate));

			return component;
		}

		template<typename T, typename std::enable_if_t<!std::is_empty_v<T>, int> = 0>
		T* tryGetComponent()
		{
			if (!hasComponent<T>()) return nullptr;
			return &GetComponent<T>();
		}

		template<typename T, typename std::enable_if_t<!std::is_empty_v<T>, int> = 0>
		T& getComponent()
		{
			return registry.get<T>(handle);
		}

		template<typename T>
		bool hasComponent()
		{
			return registry.all_of<T>(handle);
		}

		template<typename T>
		void removeComponent()
		{
			if (hasComponent<T>()) {
				registry.remove<T>(handle);
			}
		}

		void destroy()
		{
			registry.destroy(handle);
			handle = entt::null;
		}

		entt::registry& registry;
		EntityID handle;
		std::string name{};
	};
}
