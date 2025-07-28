#pragma once
#include "Engine.h"

#include "Component/Camera.h"

// stuff in here will likely get shifted to the core scene, eventually

namespace Sandbox
{
	class BaseScene : public wf::Scene
	{
	public:
		BaseScene() = default;
		~BaseScene() = default;

		/**
		 * @brief Create a "game object" with a transform component and any other essentials for visual items
		 */
		wf::Entity createObject(const wf::Vec3& position = { 0.f, 0.f, 0.f });

		/**
		 * @brief Create a perspective (default) or ortho camera. If it's the first camera, it'll also be used as the default
		 */
		wf::Entity createCamera(const wf::Vec3& position, const wf::Vec3& target, bool ortho = false, float fovOrWidth = -1.f);

		Component::Camera* getCurrentCamera();

	protected:
		Component::Camera* currentCamera{ nullptr };
	};
}