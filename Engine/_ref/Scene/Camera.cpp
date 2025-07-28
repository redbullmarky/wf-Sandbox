#include "pch.h"
#include "Camera.h"
#include "Math/Math.h"

namespace wf
{
	Mat4 Camera::getViewMatrix() const
	{
		Vec3 forward = glm::normalize(target - position);
		return Mat4{ glm::lookAt(position, position + forward, up) };
	}

	Mat4 Camera::getProjectionMatrix(float aspectRatio) const
	{
		if (orthographic) {
			float top = fovDegrees / 2.f;
			float right = top * aspectRatio;

			// Left, Right, Bottom, Top, Near, Far
			return Mat4{ glm::ortho(
				-right, right,
				-top, top,
				nearPlane, farPlane
			) };
		}
		else {
			return Mat4{ glm::perspective(glm::radians(fovDegrees), aspectRatio, nearPlane, farPlane) };
		}
	}

	Mat4 Camera::getViewProjectionMatrix(float aspectRatio) const
	{
		return getProjectionMatrix(aspectRatio) * getViewMatrix();
	}

	Camera Camera::createProjection(Vec3 pos, Vec3 target, float fovDegrees)
	{
		return {
			.position = pos,
			.target = target,
			.orthographic = false,
			.fovDegrees = fovDegrees,
		};
	}

	Camera Camera::createOrthographic(Vec3 pos, Vec3 target, float width)
	{
		return {
			.position = pos,
			.target = target,
			.orthographic = true,
			.orthoWidth = width
		};
	}

	Vec3 Camera::getForward() const
	{
		return glm::normalize(target - position);
	}

	Vec3 Camera::getUp() const
	{
		return glm::normalize(up);
	}

	Vec3 Camera::getRight() const
	{
		return glm::normalize(glm::cross(getForward(), getUp()));
	}

	void Camera::moveForward(float distance, bool moveInWorldPlane)
	{
		auto forward = getForward();

		if (moveInWorldPlane) {
			// Project vector onto world plane
			forward.y = 0.f;
			forward = glm::normalize(forward);
		}

		forward *= distance;

		position += forward;
		target += forward;
	}

	void Camera::moveUp(float distance)
	{
		auto up = getUp() * distance;

		position += up;
		target += up;
	}

	void Camera::moveRight(float distance, bool moveInWorldPlane)
	{
		auto right = getRight();

		if (moveInWorldPlane) {
			// Project vector onto world plane
			right.y = 0;
			right = glm::normalize(right);
		}

		right *= distance;

		position += right;
		target += right;
	}

	void Camera::moveToTarget(float delta)
	{
		float distance = glm::distance(position, target) + delta;
		if (distance <= 0) distance = 0.001f;

		position = target + (getForward() * -distance);
	}

	void Camera::yaw(float angle, bool rotateAroundTarget)
	{
		auto targetPosition = target - position;
		targetPosition = glm::angleAxis(angle, getUp()) * targetPosition;

		if (rotateAroundTarget) {
			position = target - targetPosition;
		}
		else {
			target = position + targetPosition;
		}
	}

	void Camera::pitch(float angle, bool lockView, bool rotateAroundTarget, bool rotateUp)
	{
		auto up = getUp();
		auto right = getRight();

		auto targetPosition = target - position;

		if (lockView) {
			float maxAngleUp = glm::degrees(glm::acos(glm::clamp(glm::dot(glm::normalize(up), glm::normalize(targetPosition)), -1.0f, 1.0f)));

			maxAngleUp -= 0.001f; // avoid numerical errors
			if (angle > maxAngleUp) angle = maxAngleUp;

			// Clamp view down
			float maxAngleDown = glm::degrees(glm::acos(glm::clamp(glm::dot(glm::normalize(-up), glm::normalize(targetPosition)), -1.0f, 1.0f)));
			maxAngleDown *= -1.0f;
			maxAngleDown += 0.001f;
			if (angle < maxAngleDown) angle = maxAngleDown;
		}

		targetPosition = glm::angleAxis(angle, right) * targetPosition;

		if (rotateAroundTarget) {
			position = target - targetPosition;
		}
		else {
			target = position + targetPosition;
		}

		if (rotateUp) {
			up = glm::angleAxis(angle, right) * up;
		}
	}

	void Camera::roll(float angle)
	{
		up = glm::angleAxis(angle, getForward()) * up;
	}
}
