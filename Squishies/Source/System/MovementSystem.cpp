#include "MovementSystem.h"
#include "Engine.h"

#include "Component/Player.h"
#include "Component/SoftBody.h"
#include "Event/DeployWeapon.h"

namespace Squishies
{
	bool MovementSystem::init()
	{
		return true;
	}

	void MovementSystem::update(float dt)
	{
		// no need for camera updates or key controls if we're trying to free-cam around
		if (wf::isKeyHeld(wf::KEY_SHIFT_LEFT)) return;

		// track the player with the camera
		entityManager->each<Component::Player, Component::SoftBody>(
			[&](const Component::Player& player, const Component::SoftBody& squishy) {

				float trackSpeed{ 3.f };

				auto& camera = *scene->getCurrentCamera();
				camera.target = glm::mix(camera.target, squishy.derivedPosition + wf::Vec3{ 0.f, 2.f, 0.f }, trackSpeed * dt);
				camera.position = { camera.target.x, camera.target.y + .5f, camera.position.z };
			});

		// integrate grenades
		entityManager->each<Component::SoftBody, Component::Player>(
			[&](wf::EntityID playerId, Component::SoftBody& squishy, Component::Player& player) {

				// we don't want to do stuff if we're working with the GUI
				if (wf::isGuiFocussed()) return;

				// move left/right
				if (wf::isKeyHeld(wf::KEY_A)) {
					applyMovement(squishy, -1.f);
				}
				else if (wf::isKeyHeld(wf::KEY_D)) {
					applyMovement(squishy, 1.f);
				}

				// duck/hide
				if (wf::isKeyHeld(wf::KEY_S)) {
					applyDuck(squishy);
				}

				// jump
				if (wf::isKeyPressed(wf::KEY_SPACE)) {
					applyJump(squishy);
				}

				// scroll through weapons
				auto wheel = wf::getMouseWheel();
				if (wheel.y != 0.f) {
					player.scrollWeapon(wheel.y > 0.f ? -1 : 1);
				}

				// fire!
				if (wf::isMouseButtonPressed(wf::BUTTON_LEFT)) {
					const auto cam = *scene->getCurrentCamera();
					deployWeapon(playerId, squishy, player, wf::getMouseWorldPosition(cam));
				}
			});
	}

	void MovementSystem::applyMovement(Component::SoftBody& squishy, float movement)
	{
		for (auto& pt : squishy.points) {
			pt.force.x += movement * 10.f;
		}
	}

	void MovementSystem::applyJump(Component::SoftBody& squishy)
	{
		for (auto& pt : squishy.points) {
			pt.force.y += 500.f;
		}
	}

	void MovementSystem::applyDuck(Component::SoftBody& squishy)
	{
		for (auto& pt : squishy.points) {
			pt.force.y -= 500.f;
		}
	}

	void MovementSystem::deployWeapon(wf::EntityID playerId, Component::SoftBody& squishy, Component::Player& player, const wf::Vec3& target)
	{
		wf::Debug::filledCircle(target, 10.f, wf::RED);

		auto pos = squishy.derivedPosition;

		auto e = event::DeployWeapon{ playerId, player.currentWeapon, squishy.derivedPosition, target, 20.f };
		eventDispatcher->dispatch<event::DeployWeapon>(e);

		// 1. get mouse position and translate to worldspace
		// 2. calculate angle from player.
		// 3. a short way from the player towards that direction, that's our start point.
		// 4. spawn the grenade.
	}
}
