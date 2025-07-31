#pragma once
#include "Engine.h"

namespace Squishies::Component
{
	struct Character
	{
		wf::Vec3 move{};			// direction to move
		wf::Vec3 lookDir{};			// direction the player is "looking" in - essentially, their aim direction rather than their transform

		bool doJump{ false };		// if the player should jump
		bool duck{ false };			// if the player is ducking

		bool doFire{ false };		// if the player is attempting to fire weapon
	};
}
