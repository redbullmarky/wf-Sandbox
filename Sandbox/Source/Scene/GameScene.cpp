#include "GameScene.h"
#include "Engine.h"

#include "Component/Geometry.h"
#include "Component/Material.h"
#include "Component/NameTag.h"

#include <imgui.h>

namespace Sandbox
{
	void GameScene::setup()
	{
		m_shadowMap = wf::wgl::createRenderTarget(2048, 2048, false, true);

		prepareScene();

		Scene::setup();
		//currentCamera = &m_lightCamera; // @temp @todo for visualising the light camera.
	}

	void GameScene::teardown()
	{
		wf::wgl::destroyRenderTarget(m_shadowMap);
	}

	void GameScene::render(float dt)
	{
		auto camera = getCurrentCamera();
		auto light = getCurrentLight();

		wf::wgl::bindRenderTarget(m_shadowMap);
		{
			getEntityManager()->each<Component::Material>(
				[&](Component::Material& mat) {
					mat.shadow.shadowPass = true;
				});

			// @todo probs need to tighten this up, but it seems this means the light camera tracks the main camera
			// likely to not waste resolution on things nowhere near the view?
			// but...it does seem to "animate", so will need to figure out why.
		/*	m_lightCamera.target = camera->position;
			m_lightCamera.position = camera->position - (light->direction * 10.f);*/

			currentCamera = &currentLight->lightCam;
			BaseScene::render(dt);

			getEntityManager()->each<Component::Material>(
				[&](Component::Material& mat) {
					mat.shadow.shadowPass = false;
				});
		}
		wf::wgl::unbindRenderTarget(m_shadowMap);

		currentCamera = camera;

		BaseScene::render(dt);

		//wf::wgl::blitRenderTarget(m_shadowMap, 0, 0, wf::getWindow().getWidth(), wf::getWindow().getHeight());
	}

	void GameScene::prepareScene()
	{
		setBackgroundColour(wf::LIGHTBLUE);

		createCamera(
			wf::Vec3{ -3.f, 5.f, 3.f },
			wf::Vec3{ 2.f, -3.f, 1.f },
			false,
			60.f
		);

		auto light = createLight(
			wf::Vec3{ 0.f, 50.f, 0.f },// { 50.f, 50.f, -50.f },
			wf::Vec3{ 0.f, 0.f, 0.f }
		);
		light->lightCam.farPlane = 100.f;


		{
			auto obj = createObject();
			obj.addComponent<Component::NameTag>("Plane");
			auto& geometry = obj.addComponent<Component::Geometry>(wf::mesh::createSimplePlane());
			auto& material = obj.addComponent<Component::Material>();
			material.diffuse.map = wf::loadTexture("resources/images/grass12.png");
			material.shadow.map = &m_shadowMap;
		}

		{
			auto obj = createObject({ -2.f, 1.f, -2.f });
			obj.addComponent<Component::NameTag>("Triangle");
			auto& geometry = obj.addComponent<Component::Geometry>(wf::mesh::createHelloTriangle());
			auto& material = obj.addComponent<Component::Material>();
			material.diffuse.map = wf::loadTexture("resources/images/brickwall.jpg");
			material.normal.map = wf::loadTexture("resources/images/brickwall_normal.jpg");
			material.shadow.map = &m_shadowMap;
		}

		{
			auto obj = createObject({ 2.f, .5f, -2.f });
			obj.addComponent<Component::NameTag>("Scruffcube");
			auto& geometry = obj.addComponent<Component::Geometry>(wf::mesh::createCube({ 1.f, 1.f, 1.f }));
			auto& material = obj.addComponent<Component::Material>();
			material.diffuse.map = wf::loadTexture("resources/images/brickwall.jpg");
			material.normal.map = wf::loadTexture("resources/images/brickwall_normal.jpg");
			material.shadow.map = &m_shadowMap;
		}

		{
			auto obj = createObject({ -2.f, .5f, 2.f });
			obj.addComponent<Component::NameTag>("Cube");
			auto& geometry = obj.addComponent<Component::Geometry>(wf::mesh::createCubeExt({ 1.f, 1.f, 1.f }));
			auto& material = obj.addComponent<Component::Material>();
			material.diffuse.map = wf::loadTexture("resources/images/gravel.jpg");
			material.normal.map = wf::loadTexture("resources/images/gravel_normal.jpg");
			material.shadow.map = &m_shadowMap;
		}

		{
			auto obj = createObject({ 2.f, 1.f, 2.f });
			obj.addComponent<Component::NameTag>("Globe");
			auto& geometry = obj.addComponent<Component::Geometry>(wf::mesh::createSphere(1.f, 25, 25));
			auto& material = obj.addComponent<Component::Material>();
			material.diffuse.map = wf::loadTexture("resources/images/earth.jpg");
			material.shadow.map = &m_shadowMap;
		}
	}

	void GameScene::renderGui(float dt)
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

			getEntityManager()->each<wf::component::Transform, Component::Material, Component::NameTag>(
				[&](wf::EntityID id, wf::component::Transform& transform, Component::Material& material, const Component::NameTag& nametag) {
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