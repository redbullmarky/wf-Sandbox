#include "pch.h"
#include "Light.h"


namespace wf
{
	LightComponent::LightComponent(const Vec3& position, const Vec3& target, LightType type)
		: type(type)
	{
		switch (type) {
		case LightType::DIRECTIONAL:
			lightCam = CameraComponent::createOrthographic(position, target, 50.f);
			lightCam.farPlane = 500.f;
			break;
		default:
			lightCam = CameraComponent::createPerspective(position, target, 90.f);
		}
	}

	Vec3 LightComponent::getDirection() const
	{
		return lightCam.getDirection();
	}

	Mat4 LightComponent::getViewProjectionMatrix() const
	{
		return lightCam.getViewProjectionMatrix();
	}
}