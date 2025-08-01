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
		static Squishy createRect(float width, float height, wf::Colour colour = wf::WHITE);

		static Squishy createSquare(float size, wf::Colour colour = wf::WHITE);

		static Squishy createCircle(float radius, int segments, int strength = 2, wf::Colour colour = wf::WHITE);

		static Squishy createEllipse(float radiusX, float radiusY, int segments, int strength = 2, wf::Colour colour = wf::WHITE);

		static Squishy createGear(float radius, int teeth, float toothDepth, wf::Colour colour = wf::DARKGREY);

	private:
		SquishyFactory() = default;
	};
}