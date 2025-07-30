#include "MovementSystem.h"
#include "Engine.h"

#include "Component/Player.h"
#include "Component/Squishy.h"

namespace Squishies
{
	bool MovementSystem::init()
	{
		return true;
	}

	void MovementSystem::update(float dt)
	{
		entityManager->each<Component::Squishy, Component::Player>(
			[&](Component::Squishy& squishy, Component::Player& player) {

				if (wf::isKeyPressed(wf::KEY_T)) {
					wf::showCursor(!wf::isCursorVisible());
					printf("Toggle cursor\n");
				}

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

				// @todo aim & fire, with mouse
				// 1. the mouse should provide the cursor
				// 2. middle wheel to select weapon
				// 3. click to fire
				auto wheel = wf::getMouseWheel();
				if (wf::isMouseButtonPressed(wf::BUTTON_LEFT)) {

				}
			});
	}

	void MovementSystem::applyMovement(Component::Squishy& squishy, float movement)
	{
		for (auto& pt : squishy.points) {
			pt.force.x += movement * 10.f;
		}
	}

	void MovementSystem::applyJump(Component::Squishy& squishy)
	{
		for (auto& pt : squishy.points) {
			pt.force.y += 500.f;
		}
	}

	void MovementSystem::applyDuck(Component::Squishy& squishy)
	{
		for (auto& pt : squishy.points) {
			pt.force.y -= 500.f;
		}
	}
}

//if (isFixed || isKinematic) return;
//		Vector2 mvec;
//		float maxJumpVel = 15.f;
//		float squish = false;
//
//		if ((dir.x < 0.f && derivedVelocity.x > -15.f) || (dir.x > 0.f && derivedVelocity.x < 15.f)) {
//			mvec.x = dir.x;
//		}
//		if ((dir.y < 0.f && derivedVelocity.y > -15.f) || (dir.y > 0.f && derivedVelocity.y < 15.f)) {
//			mvec.y = dir.y;
//
//			// temp hack for jump - set an immediate vel else it just kinda looks weird.
//			if (mvec.y < 0.f && derivedVelocity.y > -maxJumpVel) {
//				mvec.y = derivedVelocity.y - maxJumpVel;
//			}
//
//			// temp hack for squish...
//			if (mvec.y > 0.f) {
//				mvec.y = 0.f;
//				squish = true;
//			}
//		}
//
//		for (auto& p : points) {
//			p->velocity += mvec;
//
//			// temp hack for squish...
//			if (squish) {
//				p->force.y += (5.f * shapeSpringK);
//				p->velocity.x = Clamp(p->velocity.x, -3.f, 3.f); // sneaking..
//			}
//		}
