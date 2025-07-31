#pragma once
#include "Engine.h"
#include "Event/DeployWeapon.h"
#include "Event/ExplodeGrenade.h"

namespace Squishies
{
	class WeaponSystem : public wf::ISystem
	{
	public:
		using wf::ISystem::ISystem;

		virtual bool init() override;
		virtual void update(float dt) override;

	private:
		void spawnGrenade(event::DeployWeapon& detail);
		void explodeGrenade(event::ExplodeGrenade& detail);

	private:
		// grenade model, for when we need it
		std::shared_ptr<wf::Mesh> m_grenade;
	};
}