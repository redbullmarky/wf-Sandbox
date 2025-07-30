#pragma once

namespace Squishies::Component
{
	struct Grenade
	{
		wf::Vec3 force{};			// forces on the nade
		wf::Vec3 velocity{};		// current speed
		float mass{ 1.f };			// grenade mass

		float timer{ 3.f };			// how long before boom
	};
}
