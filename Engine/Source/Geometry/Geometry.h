#pragma once
#include "Math/Math.h"
#include "Misc/Colour.h"

namespace wf
{
	struct Vertex
	{
		Vec3 position{};
		Vec3 normal{};
		Colour colour{};
		Vec2 texcoord{};
		Vec4 tangent{};
	};
}