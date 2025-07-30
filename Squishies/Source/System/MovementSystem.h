#pragma once
#include "Engine.h"

#include "Component/Squishy.h"

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
		void applyMovement(Component::Squishy& squishy, float movement);
		void applyJump(Component::Squishy& squishy);
		void applyDuck(Component::Squishy& squishy);

		void selectWeapon(int dir);
		void deployWeapon();
	};
}