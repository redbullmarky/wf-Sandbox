#pragma once

namespace wf
{
	struct Bitfield
	{
		int mask{ 0 };

		void clear() { mask = 0x00; }
		void setOn(int bit) { mask |= (0x01 << (bit)); }
		void setOff(int bit) { mask &= ~(0x01 << (bit)); }
		bool getBit(int bit) const { return ((mask & (0x01 << (bit))) != 0); }
	};

	/**
	 * @brief Represents a 3D grid of bitflags used to mark spatial regions (x/y/z).
	 *
	 * Each axis (x/y/z) is a Bitfield, typically used to tag which cells or regions
	 * an object occupies in a grid-like space partitioning system.
	 *
	 * This is useful for broadphase overlap checks. If two objects don't share any
	 * bits in ANY axis, they're definitely not colliding and can be skipped.
	 *
	 * Use `miss()` to quickly check if two objects are in completely disjoint regions.
	 * Use `same()` if you want the inverse (i.e., they share at least one bit on some axis).
	 *
	 * Meant for speeding up spatial queries or reducing pairwise collision checks.
	 */
	struct Bitfields
	{
		Bitfield x;
		Bitfield y;
		Bitfield z;

		bool same(const Bitfields& other) const
		{
			return !miss(other);
		}

		/**
		 * @brief Bitfields aren't in the same region
		 * @param other Other bitfield
		 * @return true if Not in same region, false otherwise
		 */
		bool miss(const Bitfields& other) const
		{
			return (x.mask & other.x.mask) == 0 &&
				(y.mask & other.y.mask) == 0 &&
				(z.mask & other.z.mask) == 0;
		}

		void clear()
		{
			x.clear();
			y.clear();
			z.clear();
		}
	};
}