#include "MovementSystem.h"
#include "Engine.h"

#include "Component/Character.h"
#include "Component/Inventory.h"
#include "Component/SoftBody.h"
#include "Component/UserControl.h"

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
		entityManager->each<Component::UserControl, Component::SoftBody>(
			[&](const Component::SoftBody& squishy) {

				float trackSpeed{ 3.f };

				auto& camera = *scene->getCurrentCamera();
				camera.target = glm::mix(camera.target, squishy.derivedPosition + wf::Vec3{ 0.f, 2.f, 0.f }, trackSpeed * dt);
				camera.position = { camera.target.x, camera.target.y + .5f, camera.position.z };
			});

		// integrate grenades
		entityManager->each<Component::SoftBody, Component::Inventory, Component::UserControl>(
			[&](wf::EntityID playerId, Component::SoftBody& squishy, Component::Inventory& inventory) {

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
					inventory.scrollWeapon(wheel.y > 0.f ? -1 : 1);
				}

				// fire!
				if (wf::isMouseButtonPressed(wf::BUTTON_LEFT)) {
					const auto cam = *scene->getCurrentCamera();
					deployWeapon(playerId, squishy, inventory, wf::getMouseWorldPosition(cam));
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

	void MovementSystem::deployWeapon(wf::EntityID playerId, Component::SoftBody& squishy, Component::Inventory& inventory, const wf::Vec3& target)
	{
		wf::Debug::filledCircle(target, 10.f, wf::RED);

		auto pos = squishy.derivedPosition;
		auto e = event::DeployWeapon{ playerId, inventory.selectedIndex, squishy.derivedPosition, target, 20.f };

		eventDispatcher->dispatch<event::DeployWeapon>(e);
	}
}
