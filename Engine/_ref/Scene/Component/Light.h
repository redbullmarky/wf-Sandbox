#pragma once
#include "Render/Colour.h"
#include "Math/Math.h"

namespace wf::scene
{
	struct Light
	{
		Vec3 position{};
		Vec3 direction{};
		Colour colour{ WHITE };
		float ambientLevel{ .1f };
	};
}
