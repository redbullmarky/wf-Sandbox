#include "WeaponSystem.h"

#include "Component/Grenade.h"
#include "Config.h"
#include "Event/DeployWeapon.h"
#include "Event/ExplodeGrenade.h"

namespace Squishies
{
	bool WeaponSystem::init()
	{
		m_grenade = wf::mesh::createSphere(.2f, 5, 5);

		eventDispatcher->on<event::DeployWeapon>([&](event::DeployWeapon& e) {
			spawnGrenade(e);
			});
		eventDispatcher->on<event::ExplodeGrenade>([&](event::ExplodeGrenade& e) {
			explodeGrenade(e);
			});
		return true;
	}

	void WeaponSystem::update(float dt)
	{
		entityManager->each<wf::component::Transform, Component::Grenade>(
			[&](wf::EntityID id, wf::component::Transform& transform, Component::Grenade& grenade) {
				grenade.force.y += Config::get().gravity;

				grenade.velocity += (grenade.force / grenade.mass) * dt;
				transform.position += grenade.velocity * dt;

				// hard-limit bounds
				// @todo in the final one, we'll just get rid of the nades if they go out of bounds - we'll be
				// enclosed in a proper map so if we go out of that, we're not doing well.
				auto bounds = Config::get().worldBounds;

				// horizontals
				if (transform.position.x < bounds.min.x) {
					transform.position.x = bounds.min.x;
					grenade.velocity.x = -grenade.velocity.x * .8f;
				}
				else if (transform.position.x > bounds.max.x) {
					transform.position.x = bounds.max.x;
					grenade.velocity.x = -grenade.velocity.x * .8f;
				}

				if (transform.position.z < bounds.min.z) {
					transform.position.z = bounds.min.z;
					grenade.velocity.z = -grenade.velocity.z * .8f;
				}
				else if (transform.position.z > bounds.max.z) {
					transform.position.z = bounds.max.z;
					grenade.velocity.z = -grenade.velocity.z * .8f;
				}

				// vertical
				if (transform.position.y < bounds.min.y + .2f) { // bit of extra for the grenade radius
					transform.position.y = bounds.min.y + .2f;
					grenade.velocity.y = -grenade.velocity.y * .6f;
					grenade.velocity.x *= .8f;
					grenade.velocity.z *= .8f;
				}
				else if (transform.position.y > bounds.max.y) {
					transform.position.y = bounds.max.y;
					grenade.velocity.y = -grenade.velocity.y * .8f;
					grenade.velocity.x *= .8f;
					grenade.velocity.z *= .8f;
				}

				// reset forces
				grenade.force = {};
			});
	}

	void WeaponSystem::spawnGrenade(event::DeployWeapon& detail)
	{
		auto ent = scene->createObject(detail.position);

		ent.addComponent<wf::component::Geometry>(m_grenade);
		auto& material = ent.addComponent<wf::component::Material>();
		material.diffuse.colour = wf::DARKGREY;

		auto& nade = ent.addComponent<Component::Grenade>();
		nade.playerId = detail.playerId;
		nade.timer = 3.f;
		nade.velocity = glm::normalize(detail.target - detail.position) * detail.power;

		wf::createTimer(nade.timer, [ent, this](wf::CustomTimer& timer) mutable {

			auto& nade = ent.getComponent<Component::Grenade>();
			auto& pos = ent.getComponent<wf::component::Transform>();

			auto e = event::ExplodeGrenade{ ent.handle, nade.playerId, pos.position };
			eventDispatcher->dispatch<event::ExplodeGrenade>(e);

			});
	}

	void WeaponSystem::explodeGrenade(event::ExplodeGrenade& detail)
	{
		// @todo
		printf("BOOM!\n");
		entityManager->destroy(detail.id);
	}
}