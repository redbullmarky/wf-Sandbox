#pragma once
#include "Engine.h"

#include "Component/InventoryComponent.h"
#include "Component/SoftBodyComponent.h"

namespace Squishies
{
	/**
	 * @brief Handle capturing and applying movement to the Squishies from whatever source
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

		void deployWeapon(wf::Entity player, Component::SoftBody& squishy, Component::Inventory& inventory, const wf::Vec3& target);
	};
}