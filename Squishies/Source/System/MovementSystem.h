#pragma once
#include "Engine.h"

#include "Component/Player.h"
#include "Component/SoftBody.h"

namespace Squishies
{
	/**
	 * @brief Handle capturing and applying movement to the Squishies from the user.
	 *
	 * For now it's about getting things fleshed out but as much will apply to AI, we'll probably move things around later.
	 */
	class MovementSystem : public wf::ISystem
	{
	public:
		using wf::ISystem::ISystem;

		virtual bool init() override;
		virtual void update(float dt) override;

	private:
		void applyMovement(Component::SoftBody& squishy, float movement);
		void applyJump(Component::SoftBody& squishy);
		void applyDuck(Component::SoftBody& squishy);

		void deployWeapon(wf::EntityID playerId, Component::SoftBody& squishy, Component::Player& player, const wf::Vec3& target);
	};
}