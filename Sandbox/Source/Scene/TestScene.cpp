#include "TestScene.h"
#include "Engine.h"

#include <imgui.h>

namespace Sandbox
{
	bool TestScene::init()
	{
		addSystem<wf::system::RenderSystem>();
		addSystem<wf::system::CameraSystem>();

		return true;
	}

	void TestScene::setup()
	{
		m_shadowMap = wf::wgl::createRenderTarget(2048, 2048, false, true);

		prepareScene();

		Scene::setup();
		//currentCamera = &m_lightCamera; // @temp @todo for visualising the light camera.
	}

	void TestScene::teardown()
	{
		wf::wgl::destroyRenderTarget(m_shadowMap);
	}

	void TestScene::render(float dt)
	{
		auto camera = getCurrentCamera();
		auto light = getCurrentLight();

		wf::wgl::bindRenderTarget(m_shadowMap);
		{
			getEntityManager()->each<wf::component::Material>(
				[&](wf::component::Material& mat) {
					mat.shadow.shadowPass = true;
				});

			currentCamera = &currentLight->lightCam;
			wf::Scene::render(dt);

			getEntityManager()->each<wf::component::Material>(
				[&](wf::component::Material& mat) {
					mat.shadow.shadowPass = false;
				});
		}
		wf::wgl::unbindRenderTarget(m_shadowMap);

		currentCamera = camera;

		wf::Scene::render(dt);

		//wf::wgl::blitRenderTarget(m_shadowMap, 0, 0, wf::getWindow().getWidth(), wf::getWindow().getHeight());
	}

	void TestScene::prepareScene()
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
		auto brickTex = wf::loadTexture("resources/images/brickwall.jpg");
		auto brickNorm = wf::loadTexture("resources/images/brickwall_normal.jpg");
		auto waterTex = wf::loadTexture("resources/images/water2.jpg");
		auto gravelTex = wf::loadTexture("resources/images/gravel.jpg");
		auto gravelNorm = wf::loadTexture("resources/images/gravel_normal.jpg");
		auto earthTex = wf::loadTexture("resources/images/earth.jpg");
		auto scuffyNorm = wf::loadTexture("resources/images/tileable-TT7002066_nm.png");

		{
			auto obj = createObject();
			obj.addComponent<wf::component::NameTag>("Plane");
			auto& geometry = obj.addComponent<wf::component::Geometry>(wf::mesh::createSimplePlane());
			auto& material = obj.addComponent<wf::component::Material>();
			material.diffuse.map = grassTex;
			material.shadow.map = &m_shadowMap;
		}

		// ROW
		{
			auto obj = createObject({ -2.f, 1.f, -4.f });
			obj.addComponent<wf::component::NameTag>("Triangle");
			auto& geometry = obj.addComponent<wf::component::Geometry>(wf::mesh::createHelloTriangle());
			auto& material = obj.addComponent<wf::component::Material>();
			material.diffuse.map = brickTex;
			material.normal.map = brickNorm;
			material.shadow.map = &m_shadowMap;
		}

		{
			auto obj = createObject({ 2.f, .5f, -4.f });
			obj.addComponent<wf::component::NameTag>("Scruffcube");
			auto& geometry = obj.addComponent<wf::component::Geometry>(wf::mesh::createCube({ 1.f, 1.f, 1.f }));
			auto& material = obj.addComponent<wf::component::Material>();
			material.diffuse.map = brickTex;
			material.normal.map = brickNorm;
			material.shadow.map = &m_shadowMap;
		}

		{
			auto obj = createObject({ 6.f, .5f, -4.f });
			obj.addComponent<wf::component::NameTag>("Waterbox");
			auto& geometry = obj.addComponent<wf::component::Geometry>(wf::mesh::createCubeExt({ 1.f, 1.f, 1.f }));
			auto& material = obj.addComponent<wf::component::Material>();
			material.diffuse.map = waterTex;
			material.specular.intensity = 1.5f;
			material.shadow.map = &m_shadowMap;
		}

		// ROW
		{
			auto obj = createObject({ -2.f, .5f, 0.f });
			obj.addComponent<wf::component::NameTag>("Cube");
			auto& geometry = obj.addComponent<wf::component::Geometry>(wf::mesh::createCubeExt({ 1.f, 1.f, 1.f }));
			auto& material = obj.addComponent<wf::component::Material>();
			material.diffuse.map = gravelTex;
			material.normal.map = gravelNorm;
			material.shadow.map = &m_shadowMap;
		}

		{
			auto obj = createObject({ 2.f, 1.f, 0.f });
			obj.addComponent<wf::component::NameTag>("Globe");
			auto& geometry = obj.addComponent<wf::component::Geometry>(wf::mesh::createSphere(1.f, 25, 25));
			auto& material = obj.addComponent<wf::component::Material>();
			material.diffuse.map = earthTex;
			material.shadow.map = &m_shadowMap;
		}

		// ROW
		{
			auto obj = createObject({ -2.f, .5f, 4.f });
			obj.addComponent<wf::component::NameTag>("Cube 2");
			auto& geometry = obj.addComponent<wf::component::Geometry>(wf::mesh::createCubeExt({ 1.f, 1.f, 1.f }));
			auto& material = obj.addComponent<wf::component::Material>();
			material.diffuse.map = brickTex;
			material.normal.map = brickNorm;
			material.shadow.map = &m_shadowMap;
		}

		{
			auto obj = createObject({ 2.f, 1.f, 4.f });
			obj.addComponent<wf::component::NameTag>("Sphere");
			auto& geometry = obj.addComponent<wf::component::Geometry>(wf::mesh::createSphere(1.f, 25, 25));
			auto& material = obj.addComponent<wf::component::Material>();
			material.diffuse.colour = wf::RED;
			material.normal.map = scuffyNorm;
			material.specular.intensity = 1.5f;
			material.shadow.map = &m_shadowMap;
		}
	}

	void TestScene::renderGui(float dt)
	{
		auto& camera = *getCurrentCamera();
		auto& light = *getCurrentLight();

		ImGui::Begin("Sandbox");
		{
			ImGui::Text("ImGui FPS: %.2f", ImGui::GetIO().Framerate);

			ImGui::PushID("Light");
			{
				ImGui::SeparatorText("Light");
				ImGui::DragFloat3("Pos", &light.lightCam.position.x);
				ImGui::DragFloat3("Target", &light.lightCam.target.x, .1f);
				ImGui::Text("Dir: %.2f %.2f %.2f", light.getDirection().x, light.getDirection().y, light.getDirection().z);
				ImGui::DragFloat("Nearplane", &light.lightCam.nearPlane, .1f, .1f, 1000.f);
				ImGui::DragFloat("Farplane", &light.lightCam.farPlane, .1f, .1f, 1000.f);
				ImGui::DragFloat("Width", &light.lightCam.orthoWidth, 1.f, 1.f, 1000.f);
				ImGui::SliderFloat("Ambient", &light.ambientLevel, 0.f, 1.f);
			}
			ImGui::PopID();

			ImGui::PushID("Camera");
			{
				ImGui::SeparatorText("Camera");
				ImGui::DragFloat3("Pos", &camera.position.x);
				ImGui::DragFloat3("Target", &camera.target.x);
			}
			ImGui::PopID();

			getEntityManager()->each<wf::component::Transform, wf::component::Material, wf::component::NameTag>(
				[&](wf::EntityID id, wf::component::Transform& transform, wf::component::Material& material, const wf::component::NameTag& nametag) {
					ImGui::PushID(static_cast<int>(id));
					ImGui::SeparatorText(nametag.name.c_str());

					ImGui::DragFloat3("Pos", &transform.position.x);
					ImGui::DragFloat3("Rot", &transform.rotation.x);

					ImGui::SliderFloat("Norm. strength", &material.normal.strength, -3.f, 3.f);

					ImGui::SliderFloat("Spec. intesity", &material.specular.intensity, 0.f, 2.f);
					ImGui::SliderFloat("Spec. shine", &material.specular.shininess, 0.f, 128.f);

					ImGui::PopID();
				});

		}
		ImGui::End();
	}
}