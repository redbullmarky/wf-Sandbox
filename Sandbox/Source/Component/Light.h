#pragma once
#include "Engine.h"

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
