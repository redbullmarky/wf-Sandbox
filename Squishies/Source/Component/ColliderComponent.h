#pragma once
#include "Config.h"

namespace Squishies::Component
{
	/**
	 * @brief Collision component.
	 *
	 * Objects should only exist in one group. The mask then determines whether it should collide
	 *		with certain groups, or 'ALL' for lazy
	 */
	struct Collider
	{
		enum Group
		{
			NONE = 0,			// collisions entirely disabled for this group
			DEFAULT = 1,		// bog-standard catchall with no specific category
			STATIC = 2,			// static objects
			KINEMATIC = 4,		// programmatically controlled objects & fixtures
			CHARACTER = 8,		// playable characters (user or AI)
			PROJECTILE = 16,	// throwables
			LIQUID = 32,		// a liquid

			// we can add more if needed, up to 32 max for u32
			ALL = -1			// for the mask - collide with everything collidable.
		};

		int collisionGroup = Group::DEFAULT;
		int collisionMask = Group::ALL;

		Collider() = default;
		Collider(int collisionGroup, int collisionMask = Group::ALL)
			: collisionGroup(collisionGroup), collisionMask(collisionMask) {
		}
	};
}

namespace Squishies
{
	using CollisionGroup = Component::Collider::Group;
}