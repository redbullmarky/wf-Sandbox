#pragma once
#include "Poly.h"

namespace Squishies
{
	/**
	 * @brief Builders for various shapes
	 *
	 * Some support additional edge points alongside the primary points. For that reason we also keep tabs on the primary
	 * points (indexes) for convenience
	 */
	class PolyFactory
	{
	public:
		static Poly createRect(float width, float height, int extraPointsX = 0, int extraPointsY = 0);

		static Poly createSquare(float size, int extraPoints = 0);

		static Poly createCircle(float radius, int segments);

		static Poly createGear(float radius, int teeth, float toothDepth);

	private:
		PolyFactory() = default;
	};
}