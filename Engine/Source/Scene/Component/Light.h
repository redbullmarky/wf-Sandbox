#pragma once
#include "Geometry/Geometry.h"
#include "Math/Math.h"
#include "Misc/Colour.h"

namespace wf::component
{
	struct Light
	{
		Vec3 position{};
		Vec3 direction{};
		Colour colour{ WHITE };
		float ambientLevel{ .1f };

		Mat4 getViewMatrix() const;
		Mat4 getProjectionMatrix() const;
		Mat4 getViewProjectionMatrix() const;
	};
}
