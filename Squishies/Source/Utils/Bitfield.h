#pragma once

namespace Squishies
{
	struct Bitfield
	{
		int mask{ 0 };

		void clear() { mask = 0x00; }
		void setOn(int bit) { mask |= (0x01 << (bit)); }
		void setOff(int bit) { mask &= ~(0x01 << (bit)); }
		bool getBit(int bit) const { return ((mask & (0x01 << (bit))) != 0); }
	};

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