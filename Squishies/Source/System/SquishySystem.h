#pragma once
#include "Engine.h"

#include "Utils/Collider.h"

namespace Squishies
{
	/**
	 * @brief Main Squishy system for handling the creation and physics of the Squishies
	 */
	class SquishySystem : public wf::ISystem
	{
	public:
		using wf::ISystem::ISystem;

		virtual bool init() override;
		virtual void update(float dt) override;
		virtual void fixedUpdate(float dt) override;

	private:
		void createSquishy(wf::Entity entity);
		void prepareAndAccumulateForces();
		void integrate(float dt);
		void hardConstraints();
		void metaUpdates();
		void handleCollisions();
		void postUpdates();

	private:
		Collider m_collider;

		// @todo we'll likely shift these out of here but it's fine for now
		float m_gravity = -9.81f;
		wf::BoundingBox m_worldBounds{};
		float m_spatialGridSize{ 32.f };
	};
}