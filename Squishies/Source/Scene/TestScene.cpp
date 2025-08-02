#include "TestScene.h"
#include "Engine.h"

#include <imgui.h>

namespace Squishies
{
	bool TestScene::init()
	{
		addSystem<wf::system::RenderSystem>();
		addSystem<wf::system::CameraSystem>();

		return wf::Scene::init();
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
		auto scuffyNorm = wf::loadTexture("resources/images/tileable-TT7002066_nm.png");

		{
			auto obj = createObject();
			obj.addComponent<wf::NameTagComponent>("Plane");
			auto& geometry = obj.addComponent<wf::GeometryComponent>(wf::mesh::createSimplePlane());
			auto& meshRenderer = obj.addComponent<wf::MeshRendererComponent>();
			meshRenderer.material.diffuse.map = grassTex;
			meshRenderer.material.shadow.map = &m_shadowMap;
		}

		{
			auto obj = createObject({ 2.f, 1.f, 0.f });
			obj.addComponent<wf::NameTagComponent>("Sphere 1");
			auto& geometry = obj.addComponent<wf::GeometryComponent>(wf::mesh::createSphere(1.f, 25, 25));
			auto& meshRenderer = obj.addComponent<wf::MeshRendererComponent>();
			meshRenderer.material.diffuse.colour = wf::ORANGE;
			meshRenderer.material.normal.map = scuffyNorm;
			meshRenderer.material.specular.intensity = 1.5f;
			meshRenderer.material.shadow.map = &m_shadowMap;
		}

		{
			auto obj = createObject({ 2.f, 1.f, 4.f });
			obj.addComponent<wf::NameTagComponent>("Sphere 2");
			auto& geometry = obj.addComponent<wf::GeometryComponent>(wf::mesh::createSphere(1.f, 25, 25));
			auto& meshRenderer = obj.addComponent<wf::MeshRendererComponent>();
			meshRenderer.material.diffuse.colour = wf::RED;
			meshRenderer.material.normal.map = scuffyNorm;
			meshRenderer.material.specular.intensity = 1.5f;
			meshRenderer.material.shadow.map = &m_shadowMap;
		}
	}

	void TestScene::renderGui(float dt)
	{
		auto& camera = *getCurrentCamera();
		auto& light = *getCurrentLight();

		ImGui::Begin("Squishies");
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

			getEntityManager()->each<wf::TransformComponent, wf::MeshRendererComponent, wf::NameTagComponent>(
				[&](wf::EntityID id, wf::TransformComponent& transform, wf::MeshRendererComponent& meshRenderer, const wf::NameTagComponent& nametag) {
					ImGui::PushID(static_cast<int>(id));
					ImGui::SeparatorText(nametag.name.c_str());

					ImGui::DragFloat3("Pos", &transform.position.x);
					ImGui::DragFloat3("Rot", &transform.rotation.x);

					ImGui::SliderFloat("Norm. strength", &meshRenderer.material.normal.strength, -3.f, 3.f);

					ImGui::SliderFloat("Spec. intesity", &meshRenderer.material.specular.intensity, 0.f, 2.f);
					ImGui::SliderFloat("Spec. shine", &meshRenderer.material.specular.shininess, 0.f, 128.f);

					ImGui::PopID();
				});

		}
		ImGui::End();
	}
}