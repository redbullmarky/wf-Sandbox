#include "GameScene.h"
#include "Engine.h"

#include "Component/Squishy.h"
#include "System/SquishySystem.h"

#include <imgui.h>

namespace Squishies
{
	bool GameScene::init()
	{
		addSystem<wf::system::RenderSystem>();
		addSystem<wf::system::CameraSystem>();
		addSystem<SquishySystem>();

		return wf::Scene::init();
	}

	void GameScene::setup()
	{
		setBackgroundColour(wf::LIGHTBLUE);

		/*createCamera(
			wf::Vec3{ 0.f, -2.f, 65.f },
			wf::Vec3{ 0.f, -2.f, 0.f },
			true,
			30.f
		);*/
		createCamera(
			wf::Vec3{ 0.f, -2.f, 15.f },
			wf::Vec3{ 0.f, -5.f, 0.f },
			false,
			50.f
		);

		auto light = createLight(
			wf::Vec3{ 20.f, 20.f, 20.f },
			wf::Vec3{ 0.f, 0.f, 0.f }
		);
		light->lightCam.orthoWidth = 40.f;

		// textures
		auto grassTex = wf::loadTexture("resources/images/grass12.png");

		{
			auto obj = createObject({ 0.f, -10.f, 0.f });
			obj.addComponent<wf::component::NameTag>("Plane");
			auto& geometry = obj.addComponent<wf::component::Geometry>(wf::mesh::createSimplePlane());
			auto& material = obj.addComponent<wf::component::Material>();
			material.diffuse.map = grassTex;
		}

		{
			auto obj = createObject({ -2.f, 5.f, 0.f });
			obj.addComponent<wf::component::NameTag>("Squishy 1");
			auto& geometry = obj.addComponent<wf::component::Geometry>(wf::mesh::createCircle(1.f, 15));
			geometry.mesh->isDynamic = true;
			auto& material = obj.addComponent<wf::component::Material>();
			material.diffuse.colour = wf::RED;
			material.diffuse.colour.a = .7f;
			material.specular.intensity = 1.5f;
			obj.addComponent<Component::Squishy>();
		}

		/*{
			auto obj = createObject({ 2.f, 5.f, 0.f });
			obj.addComponent<wf::component::NameTag>("Squishy 2");
			auto& geometry = obj.addComponent<wf::component::Geometry>(wf::mesh::createCircle(1.f, 15));
			geometry.mesh->isDynamic = true;
			auto& material = obj.addComponent<wf::component::Material>();
			material.diffuse.colour = wf::RED;
			material.specular.intensity = 1.5f;
			obj.addComponent<Component::Squishy>();
		}

		{
			auto obj = createObject({ 6.f, 5.f, 0.f });
			obj.addComponent<wf::component::NameTag>("Squishy 3");
			auto& geometry = obj.addComponent<wf::component::Geometry>(wf::mesh::createCircle(1.f, 15));
			geometry.mesh->isDynamic = true;
			auto& material = obj.addComponent<wf::component::Material>();
			material.diffuse.colour = wf::BLUE;
			material.specular.intensity = 1.5f;
			obj.addComponent<Component::Squishy>();
		}*/

		wf::Scene::setup();
	}

	void GameScene::teardown()
	{
		wf::Scene::teardown();
	}

	void GameScene::render(float dt)
	{
		wf::Scene::render(dt);
	}

	void GameScene::renderGui(float dt)
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

			getEntityManager()->each<wf::component::Transform, wf::component::Material, wf::component::NameTag>(
				[&](wf::EntityID id, wf::component::Transform& transform, wf::component::Material& material, const wf::component::NameTag& nametag) {
					auto ent = getEntityManager()->get(id);

					ImGui::PushID(static_cast<int>(id));
					ImGui::SeparatorText(nametag.name.c_str());

					ImGui::DragFloat3("Pos", &transform.position.x);
					ImGui::DragFloat3("Rot", &transform.rotation.x);

					ImGui::SliderFloat("Norm. strength", &material.normal.strength, -3.f, 3.f);

					ImGui::SliderFloat("Spec. intesity", &material.specular.intensity, 0.f, 2.f);
					ImGui::SliderFloat("Spec. shine", &material.specular.shininess, 0.f, 128.f);

					if (ent.hasComponent<Component::Squishy>()) {
						auto& squishy = ent.getComponent<Component::Squishy>();

						ImGui::Checkbox("Shape matching", &squishy.shapeMatching);
						if (squishy.shapeMatching) {
							ImGui::SliderFloat("ShapeK", &squishy.shapeMatchK, 1.f, 5000.f);
							ImGui::SliderFloat("ShapeD", &squishy.shapeMatchDamping, 1.f, 150.f);
						}
						ImGui::SliderFloat("JointK", &squishy.jointK, 1.f, 5000.f);
						ImGui::SliderFloat("JointD", &squishy.jointDamping, 1.f, 150.f);

						ImGui::Text("Derived pos: %.2f %.2f %.2f", squishy.derivedPosition.x, squishy.derivedPosition.y, squishy.derivedPosition.z);

						float lowesty = 10000.f;
						for (auto& pt : squishy.points) {
							if (pt.position.y < lowesty) {
								lowesty = pt.position.y;
							}
						}
						ImGui::Text("Lowest Y: %.2f", lowesty);
					}

					ImGui::PopID();
				});
		}
		ImGui::End();
	}
}