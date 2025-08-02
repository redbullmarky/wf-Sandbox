#include "TestRendererScene.h"
#include "Engine.h"

#include <imgui.h>

namespace Squishies
{
	bool TestRendererScene::init()
	{
		addSystem<wf::system::RenderSystem>();
		addSystem<wf::system::CameraSystem>();

		return wf::Scene::init();
	}

	void TestRendererScene::setup()
	{
		m_shadowMap = wf::wgl::createRenderTarget(2048, 2048, false, true);

		prepareScene();

		Scene::setup();
		//currentCamera = &m_lightCamera; // @temp @todo for visualising the light camera.
	}

	void TestRendererScene::teardown()
	{
		wf::wgl::destroyRenderTarget(m_shadowMap);
	}

	void TestRendererScene::render(float dt)
	{
		auto camera = getCurrentCamera();
		auto light = getCurrentLight();

		wf::wgl::bindRenderTarget(m_shadowMap);
		{
			getEntityManager()->each<wf::MeshRendererComponent>(
				[&](wf::MeshRendererComponent& renderer) {
					renderer.material.shadow.shadowPass = true;
				});

			currentCamera = &currentLight->lightCam;
			wf::Scene::render(dt);

			getEntityManager()->each<wf::MeshRendererComponent>(
				[&](wf::MeshRendererComponent& renderer) {
					renderer.material.shadow.shadowPass = false;
				});
		}
		wf::wgl::unbindRenderTarget(m_shadowMap);

		currentCamera = camera;

		wf::Scene::render(dt);
	}

	void TestRendererScene::prepareScene()
	{
		setBackgroundColour(wf::LIGHTBLUE);

		createCamera(
			wf::Vec3{ -3.f, 5.f, 3.f },
			wf::Vec3{ 2.f, -3.f, 1.f },
			false,
			60.f
		);

		auto light = createLight(
			wf::Vec3{ 20.f, 50.f, 20.f },
			wf::Vec3{ 0.f, 0.f, 0.f }
		);
		light->lightCam.orthoWidth = 40.f;

		// textures
		auto grassTex = wf::loadTexture("resources/images/grass12.png");
		auto scuffyNorm = wf::loadTexture("resources/images/tileable-TT7002066_nm.png");

		// sphere
		auto sphere = wf::mesh::createSphere(1.f, 25, 25);

		{
			auto obj = createObject();
			obj.addComponent<wf::NameTagComponent>("Plane");
			auto& geometry = obj.addComponent<wf::GeometryComponent>(wf::mesh::createSimplePlane());
			auto& meshRenderer = obj.addComponent<wf::MeshRendererComponent>();
			meshRenderer.material = wf::createPhongMaterial();
			meshRenderer.material.diffuse.map = grassTex;
			meshRenderer.material.shadow.map = m_shadowMap;
		}

		{
			auto obj = createObject({ 2.f, 1.f, 0.f });
			obj.addComponent<wf::NameTagComponent>("Sphere 1");
			auto& geometry = obj.addComponent<wf::GeometryComponent>(sphere);
			auto& meshRenderer = obj.addComponent<wf::MeshRendererComponent>();
			meshRenderer.material = wf::createPhongMaterial();
			meshRenderer.material.diffuse.colour = wf::ORANGE;
			meshRenderer.material.normal.map = scuffyNorm;
			meshRenderer.material.specular.intensity = 1.5f;
			meshRenderer.material.shadow.map = m_shadowMap;
		}

		{
			auto obj = createObject({ 2.f, 1.f, 4.f });
			obj.addComponent<wf::NameTagComponent>("Sphere 2");
			auto& geometry = obj.addComponent<wf::GeometryComponent>(sphere);
			auto& meshRenderer = obj.addComponent<wf::MeshRendererComponent>();
			meshRenderer.material = wf::createPhongMaterial();
			meshRenderer.material.diffuse.colour = wf::RED;
			meshRenderer.material.normal.map = scuffyNorm;
			meshRenderer.material.specular.intensity = 1.5f;
			meshRenderer.material.shadow.map = m_shadowMap;
		}

		{
			auto obj = createObject({ -2.f, 1.f, 2.f });
			obj.addComponent<wf::NameTagComponent>("Sphere 3");
			auto& geometry = obj.addComponent<wf::GeometryComponent>(sphere);
			auto& meshRenderer = obj.addComponent<wf::MeshRendererComponent>();
			meshRenderer.material = wf::createPhongMaterial();
			meshRenderer.material.diffuse.colour = wf::BLUE;
			meshRenderer.material.normal.map = scuffyNorm;
			meshRenderer.material.specular.intensity = 1.5f;
			meshRenderer.material.shadow.map = m_shadowMap;
		}
	}
}