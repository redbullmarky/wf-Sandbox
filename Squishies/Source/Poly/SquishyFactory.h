#pragma once
#include "Squishy.h"

/**
 * @brief Whilst the PolyFactory is about primitives, the SquishyFactory is about taking those a step further and providing joints, strength, etc.
 */
namespace Squishies
{
	class SquishyFactory
	{
	public:
		static Squishy createRect(float width, float height);

		static Squishy createSquare(float size);

		static Squishy createCircle(float radius, int segments);

		static Squishy createGear(float radius, int teeth, float toothDepth);

	private:
		SquishyFactory() = default;
	};
}