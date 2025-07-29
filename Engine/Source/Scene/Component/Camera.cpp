#include "pch.h"
#include "Camera.h"
#include "Core/Core.h"

namespace wf::component
{
	[[nodiscard]] Camera Camera::createPerspective(Vec3 position, Vec3 target, float fovDegrees)
	{
		return {
			.position = position,
			.target = target,
			.orthographic = false,
			.fovDegrees = fovDegrees,
		};
	}

	[[nodiscard]] Camera Camera::createOrthographic(Vec3 position, Vec3 target, float width)
	{
		return {
			.position = position,
			.target = target,
			.orthographic = true,
			.orthoWidth = width
		};
	}

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

	void Camera::updateFree(float dt, float moveSpeed, float lookSpeed, float zoomSpeed)
	{
		// base speeds until we figure out stuff
		float moveBase{ 3.f * dt };
		float lookBase{ 10.f * dt };
		float zoomBase{ 250.f * dt };

		// Calculate forward and right from target - position
		Vec3 forward = getForward();
		Vec3 right = getRight();

		// Mouse look: rotate target around position
		Vec2 delta = wf::getMouseDelta() * .002f * lookSpeed;
		if (delta.x != 0.0f || delta.y != 0.0f) {
			// Horizontal rotation (Y axis)
			glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), -delta.x, up);
			forward = glm::vec3(rotY * glm::vec4(forward, 0.0f));

			// Vertical rotation (right axis)
			glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), -delta.y, right);
			forward = glm::vec3(rotX * glm::vec4(forward, 0.0f));

			target = position + forward;
		}

		// WASD movement
		if (wf::isKeyHeld(SDL_SCANCODE_W)) {
			position += forward * moveBase * moveSpeed;
		}
		if (wf::isKeyHeld(SDL_SCANCODE_S)) {
			position -= forward * moveBase * moveSpeed;
		}
		if (wf::isKeyHeld(SDL_SCANCODE_A)) {
			position -= right * moveBase * moveSpeed;
		}
		if (wf::isKeyHeld(SDL_SCANCODE_D)) {
			position += right * moveBase * moveSpeed;
		}

		// Vertical movement
		if (wf::isKeyHeld(SDL_SCANCODE_SPACE)) {
			position += up * moveBase * moveSpeed;
		}
		if (wf::isKeyHeld(SDL_SCANCODE_LCTRL)) {
			position -= up * moveBase * moveSpeed;
		}

		// Update target after movement
		target = position + forward;

		// Mouse wheel zoom (FOV or ortho width)
		Vec2 wheel = wf::getMouseWheel();
		if (wheel.y != 0.0f) {
			if (orthographic) {
				orthoWidth -= wheel.y * zoomBase * zoomSpeed;
				orthoWidth = std::max(0.1f, orthoWidth);
			}
			else {
				fovDegrees -= wheel.y * zoomBase * zoomSpeed;
				fovDegrees = glm::clamp(fovDegrees, 10.0f, 150.0f);
			}
		}
	}
}