#pragma once
#include "Engine.h"

namespace Squishies::event
{
	struct Explosion
	{
		wf::Vec3 position;
		float radius{};
		float power{ 1.f };

		Explosion(const wf::Vec3& position, float radius, float power)
			: position(position), radius(radius), power(power) {
		}
	};
}