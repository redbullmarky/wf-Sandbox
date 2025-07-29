#pragma once
#include "Camera.h"
#include "Geometry/Geometry.h"
#include "Misc/Colour.h"

namespace wf::component
{
	enum class LightType
	{
		DIRECTIONAL = 0,
		POINT,
		SPOT
	};

	struct Light
	{
		LightType type{ LightType::DIRECTIONAL }; // @todo currently only directional works anyway
		Colour colour{ WHITE };
		float ambientLevel{ .1f };

		Camera lightCam;

		Light(const Vec3& position, const Vec3& target, LightType type = LightType::DIRECTIONAL);

		Vec3 getDirection() const;
		Mat4 getViewProjectionMatrix() const;
	};
}
