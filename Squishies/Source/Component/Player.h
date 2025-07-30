#pragma once

// @todo inventory of each thing and count

namespace Squishies::Component
{
	struct Player
	{
		int currentWeapon{ 0 };
		int numWeapons{ 3 };

		void scrollWeapon(int dir)
		{
			currentWeapon = (currentWeapon + dir) % numWeapons;
			if (currentWeapon < 0)
				currentWeapon += numWeapons;
		}
	};
}
