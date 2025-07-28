#include "BaseScene.h"

#include "Component/Transform.h"

namespace Sandbox
{
	[[nodiscard]] wf::Entity BaseScene::createObject(const wf::Vec3& position)
	{
		auto ob = createEmptyObject();
		ob.addComponent<Component::Transform>(position);
		return ob;
	}

	wf::Entity Sandbox::BaseScene::createCamera(const wf::Vec3& position, const wf::Vec3& target, bool ortho, float fovOrWidth)
	{
		auto object = createObject(position);

		Component::Camera cam;

		if (ortho) {
			cam = Component::Camera::createOrthographic(
				position,
				target,
				fovOrWidth > 0.f ? fovOrWidth : 10.f
			);
		}
		else {
			cam = Component::Camera::createPerspective(
				position,
				target,
				fovOrWidth > 0.f ? fovOrWidth : 60.f
			);
		}

		auto& camera = object.addComponent<Component::Camera>(cam);
		if (!currentCamera) {
			currentCamera = &camera;
		}

		return object;
	}

	Component::Camera* Sandbox::BaseScene::getCurrentCamera()
	{
		return currentCamera;
	}
}