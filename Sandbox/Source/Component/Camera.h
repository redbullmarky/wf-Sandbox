#pragma once
#include "Engine.h"

namespace Sandbox::Component
{
	struct Camera
	{
		wf::Vec3 position{};
		wf::Vec3 target{};
		wf::Vec3 up{ 0.f, 1.f, 0.f };
		bool orthographic = false;

		// Parameters for projection
		float fovDegrees = 60.f;
		float nearPlane = 0.1f;
		float farPlane = 10000.f;

		// For ortho:
		float orthoWidth = 10.f;

		static Camera createPerspective(wf::Vec3 position, wf::Vec3 target, float fovDegrees = 60.f);
		static Camera createOrthographic(wf::Vec3 position, wf::Vec3 target, float width);

		wf::Mat4 getViewMatrix() const;
		wf::Mat4 getProjectionMatrix(float aspectRatio) const;
		wf::Mat4 getViewProjectionMatrix(float aspectRatio) const;


		wf::Vec3 getForward() const;
		wf::Vec3 getUp() const;
		wf::Vec3 getRight() const;

		void moveForward(float distance, bool moveInWorldPlane);
		void moveUp(float distance);
		void moveRight(float distance, bool moveInWorldPlane);
		void moveToTarget(float delta);

		void yaw(float angle, bool rotateAroundTarget);
		void pitch(float angle, bool lockView, bool rotateAroundTarget, bool rotateUp);
		void roll(float angle);
	};
}