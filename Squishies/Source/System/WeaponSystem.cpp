#include "WeaponSystem.h"

#include "Component/Grenade.h"
#include "Component/SoftBody.h"
#include "Config.h"
#include "Event/DeployWeapon.h"
#include "Event/ExplodeGrenade.h"
#include "Event/Explosion.h"

#include "Event/SplitSquishyEvent.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

namespace Squishies
{
	bool WeaponSystem::init()
	{
		m_grenade = wf::mesh::createSphere(.2f, 5, 5);

		eventDispatcher->on<event::DeployWeapon>([&](event::DeployWeapon& e) {
			spawnGrenade(e);
			});
		eventDispatcher->on<event::Explosion>([&](event::Explosion& e) {
			explode(e);
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

				auto p1 = wf::Debug::screenPos(wf::Vec3(transform.position.x, 0, 0));
				auto p2 = wf::Debug::screenPos(wf::Vec3(transform.position.x + grenade.blastRadius, 0, 0));
				auto dist = glm::length(p1 - p2);
				wf::Debug::circle(transform.position, dist, wf::RED);
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
		nade.timer = 2.f;
		nade.velocity = glm::normalize(detail.target - detail.position) * detail.power;

		wf::createTimer(nade.timer, [ent, this](wf::CustomTimer& timer) mutable {

			auto& nade = ent.getComponent<Component::Grenade>();
			auto& pos = ent.getComponent<wf::component::Transform>();

			// trigger the explosion and remove the grenade
			auto e = event::Explosion(pos.position, nade.blastRadius, 1.f);
			eventDispatcher->dispatch<event::Explosion>(e);
			entityManager->destroy(ent.handle);

			});
	}

	void WeaponSystem::explode(event::Explosion& detail)
	{
		entityManager->each<Component::SoftBody>(
			[&](wf::EntityID playerId, Component::SoftBody& softbody) {

				// 1. see if the blast radius reaches our bounding box
				auto dir = glm::normalize(softbody.derivedPosition - detail.position); // direction from nade to player
				auto edgePt = detail.position + dir * detail.radius; // point on edge of the blast radius

				if (!softbody.boundingBox.contains(edgePt)) return; // quick exit if we're nowhere near

				// 2. check the points
				std::vector<std::pair<unsigned int, float>> hitPoints;

				for (unsigned int i = 0; i < softbody.points.size(); i++) {
					float distSq = glm::length2(softbody.points[i].position - detail.position);

					if (distSq <= detail.radius * detail.radius) {
						hitPoints.push_back({ i, distSq });
					}
				}

				// 3. if we have points, dispatch the event
				if (hitPoints.size()) {
					event::SplitSquishy e(playerId, hitPoints, detail.position, detail.power);
					eventDispatcher->dispatch<event::SplitSquishy>(e);

					// @todo do something more devestating than a printf log. plus it'd be the listener that does this stuff not here.
					printf("GOTCHA!\n");
					entityManager->destroy(playerId);
				}
			});
	}
}