#pragma once
#include "Math/Math.h"
#include "Misc/Colour.h"

#include <memory>

namespace wf
{
	struct Mesh;

	namespace mesh
	{
		std::shared_ptr<Mesh> createSimplePlane(float size = 50.f);

		/**
		 * @brief Generate a simple cube with 8 vertices
		 */
		std::shared_ptr<Mesh> createCube(const Vec3& dimensions, const Colour& colour = WHITE);

		/**
		 * @brief Generate an extended cube with 24 vertices so that the faces are entirely separate
		 */
		std::shared_ptr<Mesh> createCubeExt(const Vec3& dimensions, const Colour& colour = WHITE);

		std::shared_ptr<Mesh> createSphere(float radius, int rings, int slices);
		std::shared_ptr<Mesh> createSphere2(float radius, int rings, int slices);

		/**
		 * @brief A simple, coloured test "hello world" triangle
		 */
		std::shared_ptr<Mesh> createHelloTriangle();
	}
}