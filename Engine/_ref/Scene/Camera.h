#pragma once
#include "Math/Math.h"

namespace wf
{
	/**
	 * @brief Simple camera class; much of the move logic taken from the awesome Raylib
	 *		for convenience. https://github.com/raysan5/raylib/blob/master/src/rcamera.h
	 */
	struct Camera
	{
		Vec3 position{};
		Vec3 target{};
		Vec3 up = Vec3(0.f, 1.f, 0.f);
		bool orthographic = false;

		// Parameters for projection
		float fovDegrees = 60.f;
		float nearPlane = 0.1f;
		float farPlane = 10000.f;

		// For ortho:
		float orthoWidth = 10.f;

		Mat4 getViewMatrix() const;
		Mat4 getProjectionMatrix(float aspectRatio) const;
		Mat4 getViewProjectionMatrix(float aspectRatio) const;

		static Camera createProjection(Vec3 pos, Vec3 target, float fovDegrees = 60.f);
		static Camera createOrthographic(Vec3 pos, Vec3 target, float width);

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
	};
}
