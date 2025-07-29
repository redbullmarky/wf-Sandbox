#include "pch.h"
#include "Light.h"

#include "Core/Core.h"
#include "Geometry/Geometry.h"

namespace wf::component
{
	Mat4 Light::getViewMatrix() const
	{
		return Mat4{ glm::lookAt(position, position + direction, Vec3{ 0.f, 1.f, 0.f }) };
	}

	Mat4 Light::getProjectionMatrix() const
	{
		// @todo assumes orthographic

		float top = 30.f / 2.f;
		float right = top * wf::getAspectRatio();

		return Mat4{ glm::ortho(
			-right, right,
			-top, top,
			.1f, 10000.f
		) };
	}

	Mat4 Light::getViewProjectionMatrix() const
	{
		return getProjectionMatrix() * getViewMatrix();
	}
}