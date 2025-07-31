#pragma once
#include "Engine.h"

#include <vector>

namespace Squishies::Component
{
	enum class WeaponType
	{
		NADE = 0,
		MINE,
		BALLOON
	};

	struct InventoryItem
	{
		WeaponType type;			// what this is
		int count;					// number of item, or -1 for infinite

		static std::string getWeaponName(WeaponType type)
		{
			switch (type) {
			case WeaponType::NADE:
				return "Grenade";
				break;
			case WeaponType::MINE:
				return "Landmine";
				break;
			case WeaponType::BALLOON:
				return "Balloon";
				break;
			default:
				return "[Unknown]";
			}
		}
	};

	struct Inventory
	{
		std::vector<InventoryItem> items;
		int selectedIndex{ 0 };

		/**
		 * @brief Cycle through
		 */
		void scrollWeapon(int dir)
		{
			if (items.empty()) return;

			int newIndex = selectedIndex;
			int itemCount = static_cast<int>(items.size());
			for (int i = 0; i < itemCount; ++i) {
				newIndex = (newIndex + dir + itemCount) % itemCount;
				if (items[newIndex].count > 0) {
					selectedIndex = newIndex;
					return;
				}
			}
		}

		void select(WeaponType type)
		{
			for (int i = 0; i < static_cast<int>(items.size()); i++) {
				if (items[i].type == type) {
					selectedIndex = i;
					return;
				}
			}
		}
	};
}
