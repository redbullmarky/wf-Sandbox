#include "GameScene.h"
#include "Engine.h"

#include "Component/Geometry.h"
#include "Component/Material.h"
#include "Helpers.h"

namespace Sandbox
{
	void GameScene::setup()
	{
		prepareScene();
		Scene::setup();
	}

	void GameScene::prepareScene()
	{
		createCamera(
			wf::Vec3{ -1.f, 1.f, 2.f },
			wf::Vec3{ 0.f, 0.f, 0.f },
			false,
			60.f
		);

		{
			auto obj = createObject();
			auto& geometry = obj.addComponent<Component::Geometry>(createHelloTriangle());
			auto& material = obj.addComponent<Component::Material>();
			material.diffuse.map = wf::loadTexture("resources/images/brickwall.jpg");
			material.normal.map = wf::loadTexture("resources/images/brickwall_normal.jpg");
			material.specular.intensity = 5.f;
			material.specular.colour = wf::RED;

			//material.diffuse.colour = wf::WHITE;
		}
	}
}