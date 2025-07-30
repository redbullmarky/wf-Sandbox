#include "GameScene.h"
#include "Engine.h"

#include "Component/Player.h"
#include "Component/Squishy.h"
#include "System/SquishySystem.h"
#include "System/MovementSystem.h"
#include "System/WeaponSystem.h"

#include <imgui.h>
#include <glm/glm.hpp>

namespace Squishies
{
	bool GameScene::init()
	{
		addSystem<wf::system::RenderSystem>();
		addSystem<wf::system::CameraSystem>();
		addSystem<SquishySystem>();
		addSystem<MovementSystem>();
		addSystem<WeaponSystem>();

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

		// prepare the debugger to track the camera
		wf::Debug::instance().setCamera(getCurrentCamera());

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
			auto& geometry = obj.addComponent<wf::component::Geometry>(wf::mesh::createSimplePlane(100.f));
			auto& material = obj.addComponent<wf::component::Material>();
			material.diffuse.map = grassTex;
		}

		int points = 15;		// number of points on the squishies
		float radius = 1.f;	// size

		{
			auto obj = createObject({ -2.f, -5.f, 0.f });
			obj.addComponent<wf::component::NameTag>("Squishy 1");
			auto& material = obj.addComponent<wf::component::Material>();
			material.diffuse.colour = wf::RED;
			material.specular.intensity = 1.5f;
			obj.addComponent<Component::Squishy>(radius, points);
			obj.addComponent<Component::Player>();
		}

		{
			auto obj = createObject({ -1.2f, -3.f, 0.f });
			obj.addComponent<wf::component::NameTag>("Squishy 2");
			auto& material = obj.addComponent<wf::component::Material>();
			material.diffuse.colour = wf::BLUE;
			material.specular.intensity = 1.5f;
			obj.addComponent<Component::Squishy>(radius, points);
		}
		/*
		{
			auto obj = createObject({ 6.f, 5.f, 0.f });
			obj.addComponent<wf::component::NameTag>("Squishy 3");
			auto& material = obj.addComponent<wf::component::Material>();
			material.diffuse.colour = wf::BLUE;
			material.specular.intensity = 1.5f;
			obj.addComponent<Component::Squishy>(radius, points);
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

	static ImVec2 getScreenPos(const wf::Vec3& pos, const wf::component::Camera& camera)
	{
		glm::vec4 clipSpace = camera.getViewProjectionMatrix().matrix * glm::vec4(pos, 1.0f);
		if (clipSpace.w <= 0.0f) return{}; // behind camera

		glm::vec3 ndc = glm::vec3(clipSpace) / clipSpace.w; // -1 to +1
		glm::vec2 screenPos{};
		screenPos.x = (ndc.x * 0.5f + 0.5f) * wf::getWindow().getWidth();
		screenPos.y = (1.0f - (ndc.y * 0.5f + 0.5f)) * wf::getWindow().getHeight();
		return { screenPos.x, screenPos.y };
	}

	void GameScene::renderGui(float dt)
	{
		auto& camera = *getCurrentCamera();
		auto& light = *getCurrentLight();

		ImGui::Begin("Squishies");
		{
			ImGui::Text("ImGui FPS: %.2f", ImGui::GetIO().Framerate);
			ImGui::Text("Gui focussed: %s", wf::isGuiFocussed() ? "Yes" : "No");
			ImGui::Text("Cursor visible: %s", wf::isCursorVisible() ? "Yes" : "No");

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

						// wf::Debug::filledCircle(squishy.derivedPosition, 5.f, wf::YELLOW); // derived position
						// wf::Debug::rect(squishy.boundingBox, 2.f, wf::WHITE); // bounding box
					}

					ImGui::PopID();
				});
		}
		ImGui::End();

		// any debug points we have, render now..
		wf::Debug::render();
	}
}
