#pragma once
#include "Math/Math.h"
#include "Render/Colour.h"

#include <cstdint>
#include <vector>

namespace wf
{
	struct Vertex
	{
		Vec3 position;
		Vec3 normal;
		Colour colour;
		Vec2 texcoord;
		Vec4 tangent;
	};

	struct Mesh
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		bool isDynamic{ false };

		unsigned int vao{};
		unsigned int vbo{};
		unsigned int ebo{};
	};

}