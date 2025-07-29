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
		prepareScene();
		Scene::setup();
	}

	void GameScene::prepareScene()
	{
		createCamera(
			wf::Vec3{ -3.f, 5.f, 3.f },
			wf::Vec3{ 2.f, -3.f, 1.f },
			false,
			60.f
		);

		{
			auto obj = createObject({ -2.f, 0.f, -2.f });
			obj.addComponent<Component::NameTag>("Triangle");
			auto& geometry = obj.addComponent<Component::Geometry>(wf::mesh::createHelloTriangle());
			auto& material = obj.addComponent<Component::Material>();
			material.diffuse.map = wf::loadTexture("resources/images/brickwall.jpg");
			material.normal.map = wf::loadTexture("resources/images/brickwall_normal.jpg");
		}

		{
			auto obj = createObject({ 2.f, 0.f, -2.f });
			obj.addComponent<Component::NameTag>("Scruffcube");
			auto& geometry = obj.addComponent<Component::Geometry>(wf::mesh::createCube({ 1.f, 1.f, 1.f }));
			auto& material = obj.addComponent<Component::Material>();
			material.diffuse.map = wf::loadTexture("resources/images/brickwall.jpg");
			material.normal.map = wf::loadTexture("resources/images/brickwall_normal.jpg");
		}

		{
			auto obj = createObject({ -2.f, 0.f, 2.f });
			obj.addComponent<Component::NameTag>("Cube");
			auto& geometry = obj.addComponent<Component::Geometry>(wf::mesh::createCubeExt({ 1.f, 1.f, 1.f }));
			auto& material = obj.addComponent<Component::Material>();
			material.diffuse.map = wf::loadTexture("resources/images/gravel.jpg");
			material.normal.map = wf::loadTexture("resources/images/gravel_normal.jpg");
		}

		{
			auto obj = createObject({ 2.f, 0.f, 2.f });
			obj.addComponent<Component::NameTag>("Globe");
			auto& geometry = obj.addComponent<Component::Geometry>(wf::mesh::createSphere(1.f, 25, 25));
			auto& material = obj.addComponent<Component::Material>();
			material.diffuse.map = wf::loadTexture("resources/images/earth.jpg");
		}
	}

	void GameScene::renderGui(float dt)
	{
		auto& camera = *getCurrentCamera();

		ImGui::Begin("Sandbox");
		{
			ImGui::Text("ImGui FPS: %.2f", ImGui::GetIO().Framerate);

			ImGui::DragFloat3("CamPos", &camera.position.x);
			ImGui::DragFloat3("CamTar", &camera.target.x);

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