#include "pch.h"
#include "EntityManager.h"

#include <stdexcept>

namespace wf
{
	entt::registry& EntityManager::getRegistry()
	{
		return m_registry;
	}

	entt::entity EntityManager::createID()
	{
		return m_registry.create();
	}

	Entity EntityManager::create()
	{
		return Entity{ m_registry, m_registry.create() };
	}

	Entity EntityManager::createNamed(const std::string& name)
	{
		if (isValid(name)) throw std::runtime_error("Entity with name already exists: " + name);
		auto entity = create();
		entity.name = name;
		m_nameToId[name] = entity.handle;
		m_idToName[entity.handle] = name;
		return entity;
	}

	void EntityManager::retargetName(const std::string& name, EntityID newId)
	{
		if (!isValid(newId)) throw std::runtime_error("No valid entity with name: " + name);

		removeNamedLookup(name);
		m_nameToId[name] = newId;
		m_idToName[newId] = name;
	}

	Entity EntityManager::get(EntityID id)
	{
		return Entity{ m_registry, id, nameOf(id) };
	}

	Entity EntityManager::get(const std::string& name)
	{
		if (!isValid(name)) throw std::runtime_error("No valid entity with name: " + name);
		return get(m_nameToId.at(name));
	}

	std::string EntityManager::nameOf(EntityID id)
	{
		if (!m_idToName.contains(id)) return "";
		return m_idToName.at(id);
	}

	bool EntityManager::isValid(EntityID id)
	{
		return m_registry.valid(id);
	}

	bool EntityManager::isValid(const std::string& name)
	{
		return m_nameToId.contains(name) && isValid(m_nameToId.at(name));
	}

	void EntityManager::destroy(EntityID id)
	{
		removeNamedLookup(id);
		m_registry.destroy(id);
	}

	void EntityManager::destroy(const std::string& name)
	{
		if (!isValid(name)) return;
		destroy(m_nameToId.at(name));
	}

	void EntityManager::clear()
	{
		m_registry.clear();
	}

	void EntityManager::removeNamedLookup(EntityID id, bool recurse)
	{
		if (m_idToName.contains(id)) {
			if (recurse) removeNamedLookup(m_idToName.at(id), false);
			m_idToName.erase(id);
		}
	}

	void EntityManager::removeNamedLookup(std::string name, bool recurse)
	{
		if (m_nameToId.contains(name)) {
			if (recurse) removeNamedLookup(m_nameToId.at(name), false);
			m_nameToId.erase(name);
		}
	}
}
