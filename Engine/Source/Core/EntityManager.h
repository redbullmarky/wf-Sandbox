#pragma once
#include "EventDispatcher.h"

#include <entt/entt.hpp>
#include <string>
#include <unordered_map>

namespace wf
{
	using EntityID = entt::entity;

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

	/**
	 * @brief Wrapper for entt, mostly full of hacky/naive convenience functions
	 */
	class EntityManager
	{
	public:
		EntityManager() = default;
		~EntityManager() = default;

		/**
		 * @brief If required, fetch the underlaying entt registry
		 */
		entt::registry& getRegistry();

		/**
		 * @brief Create an entity, returning just the ID
		 */
		EntityID createID();

		/**
		 * @brief Create an entity, wrapped
		 */
		Entity create();

		/**
		 * @brief Create an entity, wrapped and given an easy name for lookup
		 */
		Entity createNamed(const std::string& name);

		/**
		 * @brief Moves the name to a target a different entity
		 */
		void retargetName(const std::string& name, EntityID newId);

		/**
		 * @brief Fetch a wrapped entity by id
		 */
		Entity get(EntityID id);

		/**
		 * @brief Fetch entity by name
		 */
		Entity get(const std::string& name);

		/**
		 * @brief Fetch name for entity if available, else an emtpy string
		 */
		std::string nameOf(EntityID id);

		/**
		 * @brief Determine if the entity exists/is valid
		 */
		bool isValid(EntityID id);

		/**
		 * @brief Determine if named entity exists/is valid
		 */
		bool isValid(const std::string& name);

		/**
		 * @brief Destroy an entity and its components
		 */
		void destroy(EntityID id);

		/**
		 * @brief Destroy a named entity and its components
		 * @param name
		 */
		void destroy(const std::string& name);

		/**
		 * @brief Destroy all entities and associated components
		 */
		void clear();

		/**
		 * @brief Find entities with a specific [set of] component[s] attached
		 */
		template<typename... Components>
		auto find()
		{
			return m_registry.view<Components...>();
		}

		/**
		 * @brief Convenience iterator for 'Find'
		 */
		template <typename... T, typename Func>
		void each(Func&& func)
		{
			find<T...>().each(std::forward<Func>(func));
		}

		/**
		 * @brief Returns the first entity matching the specified component types,
		 *		along with its component(s) (entity as first tuple item)
		 */
		template <typename... Components>
		auto first()
		{
			auto view = m_registry.view<Components...>();
			assert(!view.empty() && "first(): No entity found with requested components");

			auto entity = *view.begin();
			return std::tuple_cat(std::make_tuple(entity), view.get<Components...>(entity));
		}


		/**
		 * @brief Invoked when a component is added to an entity. The entity instance (Engine::Entity) is passed to the provided lambda
		 */
		template<typename Component, typename Func>
		void onCreate(Func&& func) {
			using EventType = ECSComponentEvent_Create<Component>;

			auto wrapperfunc = [func = std::forward<Func>(func), this](EventType& event) mutable {
				func(event.entity);
				};

			m_eventDispatcher.on<EventType>(std::move(wrapperfunc));
			m_registry.on_construct<Component>().connect<&EntityManager::wrapper<Component, ECSComponentEvent_Create>>(this);
		}

		/**
		 * @brief Invoked when a component is removed from an entity. The entity instance (Engine::Entity) is passed to the provided lambda
		 */
		template<typename Component, typename Func>
		void onRemove(Func&& func) {
			using EventType = ECSComponentEvent_Remove<Component>;

			auto wrapperfunc = [func = std::forward<Func>(func), this](EventType& event) mutable {
				func(event.entity);
				};

			m_eventDispatcher.on<EventType>(std::move(wrapperfunc));
			m_registry.on_destroy<Component>().connect<&EntityManager::wrapper<Component, ECSComponentEvent_Remove>>(this);
		}

	private:
		void removeNamedLookup(EntityID id, bool recurse = true);
		void removeNamedLookup(std::string name, bool recurse = true);

	private:
		/**
		 * @brief Main entt registry
		 */
		entt::registry m_registry;

		/**
		 * @brief Internal event dispatcher
		 */
		EventDispatcher m_eventDispatcher;

		/**
		 * @brief Maps for named entities
		 */
		std::unordered_map<std::string, EntityID> m_nameToId;
		std::unordered_map<EntityID, std::string> m_idToName;

		/**
		 * @brief Base event type for internal handling/delegation. i.e. a hack, probably :-p
		 */
		struct ECSComponentEvent
		{
			wf::Entity entity;
			ECSComponentEvent(wf::Entity entity) : entity(entity) {}
		};

		/**
		 * @brief A component was added to entity
		 */
		template<typename T>
		struct ECSComponentEvent_Create : public ECSComponentEvent { using ECSComponentEvent::ECSComponentEvent; };

		/**
		 * @brief A component was removed from entity
		 */
		template<typename T>
		struct ECSComponentEvent_Remove : public ECSComponentEvent { using ECSComponentEvent::ECSComponentEvent; };

		/**
		 * @brief Wrapper that gets the details from entt and forwards them on via our own dispatcher
		 */
		template<typename TComponent, template<typename> class TEventType>
		void wrapper(entt::entity id)
		{
			Entity ent = get(id);
			TEventType<TComponent> e{ ent };
			m_eventDispatcher.dispatch<TEventType<TComponent>>(e);
		}
	};
}
