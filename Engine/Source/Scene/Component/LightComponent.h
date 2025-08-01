#pragma once
#include "CameraComponent.h"
#include "Geometry/Geometry.h"
#include "Misc/Colour.h"

namespace wf
{
	enum class LightType
	{
		DIRECTIONAL = 0,
		POINT,
		SPOT
	};

	struct LightComponent
	{
		LightType type{ LightType::DIRECTIONAL }; // @todo currently only directional works anyway
		Colour colour{ WHITE };
		float ambientLevel{ .1f };

		CameraComponent lightCam;

		LightComponent(const Vec3& position, const Vec3& target, LightType type = LightType::DIRECTIONAL);

		Vec3 getDirection() const;
		Mat4 getViewProjectionMatrix() const;
	};
}
