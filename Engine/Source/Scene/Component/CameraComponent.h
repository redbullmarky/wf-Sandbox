#pragma once
#include "Geometry/Geometry.h"
#include "Math/Math.h"

namespace wf
{
	struct CameraComponent
	{
		Vec3 position{};
		Vec3 target{};
		Vec3 up{ 0.f, 1.f, 0.f };
		bool orthographic = false;

		// Parameters for projection
		float fovDegrees = 60.f;
		float nearPlane = 0.1f;
		float farPlane = 10000.f;

		// For ortho:
		float orthoWidth = 10.f;

		static CameraComponent createPerspective(Vec3 position, Vec3 target, float fovDegrees = 60.f);
		static CameraComponent createOrthographic(Vec3 position, Vec3 target, float width);

		Vec3 getDirection() const;

		Mat4 getViewMatrix() const;
		Mat4 getProjectionMatrix() const;
		Mat4 getViewProjectionMatrix() const;

		Vec3 getForward() const;
		Vec3 getUp() const;
		Vec3 getRight() const;

		void moveForward(float distance, bool moveInWorldPlane);
		void moveUp(float distance);
		void moveRight(float distance, bool moveInWorldPlane);
		void moveToTarget(float delta);

		void yaw(float angle, bool rotateAroundTarget);
		void pitch(float angle, bool lockView, bool rotateAroundTarget, bool rotateUp);
		void roll(float angle);

		void updateFree(float dt, float moveSpeed = 1.f, float lookSpeed = 1.f, float zoomSpeed = 1.f);
	};
}