#pragma once
#include "Engine.h"

namespace Squishies
{
	class SquishySystem : public wf::ISystem
	{
	public:
		using wf::ISystem::ISystem;

		virtual bool init() override;
		virtual void setup() override;
		virtual void update(float dt) override;
		virtual void fixedUpdate(float dt) override;

	private:
		void createSquishy(wf::Entity entity);
		void prepareAndAccumulateForces();
		void integrate(float dt);
		void hardConstraints();
		void metaUpdates();
		void collisionDetection();
		void collisionResponse();
		void postUpdates();

	private:
		// @todo we'll likely shift these out of here but it's fine for now
		float m_gravity = -9.81f;
		wf::BoundingBox m_worldBounds{};
	};
}