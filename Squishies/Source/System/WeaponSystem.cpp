#include "WeaponSystem.h"

#include "Component/Character.h"
#include "Component/Collider.h"
#include "Component/Grenade.h"
#include "Component/SoftBody.h"
#include "Config.h"
#include "Event/DeployWeapon.h"
#include "Event/ExplodeGrenade.h"
#include "Event/Explosion.h"
#include "Event/SplitSquishyEvent.h"
#include "Poly/Squishy.h"
#include "Poly/SquishyFactory.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

namespace Squishies
{
	bool WeaponSystem::init()
	{
		// @todo we might us an ellipse if it had actual rotation when thrown
		m_grenadeProto = std::make_shared<Squishy>(SquishyFactory::createEllipse(.25f, .25f, 9, 4, wf::DARKGREY));

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
		// visualising blast radius
		/*entityManager->each<Component::SoftBody, Component::Grenade>(
			[&](Component::SoftBody& softbody, Component::Grenade& nade) {

				auto p1 = wf::Debug::screenPos(wf::Vec3(softbody.derivedPosition.x, 0, 0));
				auto p2 = wf::Debug::screenPos(wf::Vec3(softbody.derivedPosition.x + nade.blastRadius, 0, 0));
				auto dist = glm::length(p1 - p2);
				wf::Debug::circle(softbody.derivedPosition, dist, wf::RED);
			});*/
	}

	// @todo these kind of spawnables (like players) should probably be a scene-level thing so that we keep prefabs etc all together.
	void WeaponSystem::spawnGrenade(event::DeployWeapon& detail)
	{
		auto ent = scene->createObject(detail.position);
		auto& material = ent.addComponent<wf::component::Material>();
		auto& body = ent.addComponent<Component::SoftBody>(*m_grenadeProto.get());
		auto& nade = ent.addComponent<Component::Grenade>();
		nade.playerId = detail.playerId;

		// collider
		ent.addComponent<Component::Collider>(CollisionGroup::PROJECTILE, CollisionGroup::ALL & ~(CollisionGroup::CHARACTER));

		auto vel = glm::normalize(detail.target - detail.position) * detail.power;;

		for (auto& pt : body.points) {
			pt.velocity = vel;
		}

		wf::createTimer(nade.timer, [ent, this](wf::CustomTimer& timer) mutable {

			auto& nade = ent.getComponent<Component::Grenade>();
			auto& body = ent.getComponent<Component::SoftBody>();

			// trigger the explosion and remove the grenade
			auto e = event::Explosion(body.derivedPosition, nade.blastRadius, 1.f);
			eventDispatcher->dispatch<event::Explosion>(e);
			entityManager->destroy(ent.handle);

			});
	}

	void WeaponSystem::explode(event::Explosion& detail)
	{
		entityManager->each<Component::SoftBody>(
			[&](wf::EntityID playerId, Component::SoftBody& softbody) {

				// not a player
				if (!entityManager->get(playerId).hasComponent<Component::Character>()) return;

				// 1. see if the blast radius reaches our bounding box
				const wf::Vec2 corners[4] = {
					softbody.boundingBox.min,
					{ softbody.boundingBox.max.x, softbody.boundingBox.min.y },
					softbody.boundingBox.max,
					{ softbody.boundingBox.min.x, softbody.boundingBox.max.y }
				};

				bool outside = true;
				for (const auto& corner : corners) {
					float dist2 = glm::length2(wf::Vec2(detail.position) - corner);
					if (dist2 <= detail.radius * detail.radius) {
						outside = false;
						break;
					}
				}

				if (outside) return;

				// 2. check the points
				std::vector<std::pair<unsigned int, float>> hitPoints;

				for (unsigned int i = 0; i < softbody.points.size(); i++) {
					float distSq = glm::length2(softbody.points[i].position - detail.position);

					if (distSq <= detail.radius * detail.radius) {
						hitPoints.push_back({ i, distSq });
					}
				}

				auto& name = entityManager->get(playerId).getComponent<wf::component::NameTag>();
				printf("hitpoints on %s = %zu", name.name.c_str(), hitPoints.size());

				// 3. if we have points, dispatch the event
				if (hitPoints.size()) {
					event::SplitSquishy e(playerId, hitPoints, detail.position, detail.power);
					eventDispatcher->dispatch<event::SplitSquishy>(e);

					// @todo do something more devestating than a printf log. plus it'd be the listener that does this stuff not here.
					printf("GOTCHA!\n");
					//entityManager->destroy(playerId);
				}
			});
	}
}