#include "pch.h"
#include "Light.h"


namespace wf::component
{
	Light::Light(const Vec3& position, const Vec3& target, LightType type)
		: type(type)
	{
		switch (type) {
		case LightType::DIRECTIONAL:
			lightCam = Camera::createOrthographic(position, target, 50.f);
			lightCam.farPlane = 500.f;
			break;
		default:
			lightCam = Camera::createPerspective(position, target, 90.f);
		}
	}

	Vec3 Light::getDirection() const
	{
		return lightCam.getDirection();
	}

	Mat4 Light::getViewProjectionMatrix() const
	{
		return lightCam.getViewProjectionMatrix();
	}
}