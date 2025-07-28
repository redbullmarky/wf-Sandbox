#include "GameScene.h"
#include "Engine.h"

#include "Component/Camera.h"
#include "Component/Geometry.h"
#include "Component/Material.h"
#include "Helpers.h"

namespace Sandbox
{
	bool GameScene::init()
	{
		prepareScene();

		return Scene::init();
	}

	void GameScene::shutdown()
	{
		entityManager.clear();

		Scene::shutdown();
	}

	void GameScene::update(float dt)
	{
		// printf("%.2f, %.2f, %.2f\n", config.currentCamera->position.x, config.currentCamera->position.y, config.currentCamera->position.z);
	}

	void GameScene::prepareScene()
	{
		createCamera(
			wf::Vec3{ 5.f, 5.f, 5.f },
			wf::Vec3{ 0.f, 0.f, 0.f },
			false,
			60.f
		);

		{
			auto obj = createObject();
			auto& geometry = obj.addComponent<Component::Geometry>(createHelloTriangle());
			auto& material = obj.addComponent<Component::Material>();
		}
	}
}