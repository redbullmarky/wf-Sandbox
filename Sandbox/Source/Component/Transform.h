#pragma once
#include "Engine.h"

namespace Sandbox::Component
{
	struct Transform
	{
		wf::Vec3 position{};
		wf::Vec3 rotation{};
		wf::Vec3 scale{ 1.f, 1.f, 1.f };

		Transform(const wf::Vec3& position) : position(position), rotation(wf::Vec3{}) {}
		Transform(const wf::Vec3& position, const wf::Vec3& rotation) : position(position), rotation(rotation) {}

		wf::Mat4 getTransformMatrix() const
		{
			auto transform = wf::Mat4(1.f);

			transform.translate(position);

			transform.rotate(rotation.y * DEG2RAD, wf::Vec3(0, 1, 0));
			transform.rotate(rotation.x * DEG2RAD, wf::Vec3(1, 0, 0));
			transform.rotate(rotation.z * DEG2RAD, wf::Vec3(0, 0, 1));

			transform.scale(scale);

			return transform;
		}
	};
}