#pragma once
#include "Engine.h"

#include "Event/DeployWeapon.h"
#include "Event/Explosion.h"

#include <memory>

namespace Squishies
{
	struct Squishy;

	class WeaponSystem : public wf::ISystem
	{
	public:
		using wf::ISystem::ISystem;

		virtual bool init() override;
		virtual void update(float dt) override;

	private:
		void spawnGrenade(event::DeployWeapon& detail);
		void explode(event::Explosion& detail);

	private:
		std::shared_ptr<Squishy> m_grenadeProto;
	};
}