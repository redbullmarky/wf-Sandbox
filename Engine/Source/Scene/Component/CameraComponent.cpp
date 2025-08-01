#include "pch.h"
#include "CameraComponent.h"
#include "Core/Core.h"

namespace wf
{
	[[nodiscard]] CameraComponent CameraComponent::createPerspective(Vec3 position, Vec3 target, float fovDegrees)
	{
		return {
			.position = position,
			.target = target,
			.orthographic = false,
			.fovDegrees = fovDegrees,
		};
	}

	[[nodiscard]] CameraComponent CameraComponent::createOrthographic(Vec3 position, Vec3 target, float width)
	{
		return {
			.position = position,
			.target = target,
			.orthographic = true,
			.orthoWidth = width
		};
	}

	Vec3 CameraComponent::getDirection() const
	{
		return glm::normalize(target - position);
	}

	Mat4 CameraComponent::getViewMatrix() const
	{
		Vec3 forward = glm::normalize(target - position);

		// Prevent lookAt singularity when forward is nearly parallel to up vector
		// by choosing an alternative up vector to avoid zero cross product.
		Vec3 safeUp = up;
		if (glm::abs(glm::dot(forward, glm::normalize(up))) > 0.999f) {
			safeUp = Vec3{ 0.f, 0.f, 1.f };
		}

		return Mat4{ glm::lookAt(position, position + forward, safeUp) };
	}

	Mat4 CameraComponent::getProjectionMatrix() const
	{
		if (orthographic) {
			float halfWidth = orthoWidth / 2.f;
			float halfHeight = halfWidth / wf::getAspectRatio();

			return Mat4{ glm::ortho(
				-halfWidth, halfWidth,
				-halfHeight, halfHeight,
				nearPlane, farPlane
			) };
		}
		else {
			return Mat4{ glm::perspective(glm::radians(fovDegrees), wf::getAspectRatio(), nearPlane, farPlane) };
		}
	}

	Mat4 CameraComponent::getViewProjectionMatrix() const
	{
		return getProjectionMatrix() * getViewMatrix();
	}

	Vec3 CameraComponent::getForward() const
	{
		return glm::normalize(target - position);
	}

	Vec3 CameraComponent::getUp() const
	{
		return glm::normalize(up);
	}

	Vec3 CameraComponent::getRight() const
	{
		return glm::normalize(glm::cross(getForward(), getUp()));
	}

	void CameraComponent::moveForward(float distance, bool moveInWorldPlane)
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

	void CameraComponent::moveUp(float distance)
	{
		auto up = getUp() * distance;

		position += up;
		target += up;
	}

	void CameraComponent::moveRight(float distance, bool moveInWorldPlane)
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

	void CameraComponent::moveToTarget(float delta)
	{
		float distance = glm::distance(position, target) + delta;
		if (distance <= 0) distance = 0.001f;

		position = target + (getForward() * -distance);
	}

	void CameraComponent::yaw(float angle, bool rotateAroundTarget)
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

	void CameraComponent::pitch(float angle, bool lockView, bool rotateAroundTarget, bool rotateUp)
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

	void CameraComponent::roll(float angle)
	{
		up = glm::angleAxis(angle, getForward()) * up;
	}

	void CameraComponent::updateFree(float dt, float moveSpeed, float lookSpeed, float zoomSpeed)
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
		if (wf::isKeyHeld(KEY_W)) {
			position += forward * moveBase * moveSpeed;
		}
		if (wf::isKeyHeld(KEY_S)) {
			position -= forward * moveBase * moveSpeed;
		}
		if (wf::isKeyHeld(KEY_A)) {
			position -= right * moveBase * moveSpeed;
		}
		if (wf::isKeyHeld(KEY_D)) {
			position += right * moveBase * moveSpeed;
		}

		// Vertical movement
		if (wf::isKeyHeld(KEY_SPACE)) {
			position += up * moveBase * moveSpeed;
		}
		if (wf::isKeyHeld(KEY_CTRL_LEFT)) {
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