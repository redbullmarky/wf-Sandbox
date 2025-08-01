#include "GameScene.h"
#include "Engine.h"

#include "Component/Character.h"
#include "Component/Collider.h"
#include "Component/Inventory.h"
#include "Component/SoftBody.h"
#include "Component/UserControl.h"
#include "Poly/PolyFactory.h"
#include "Poly/SquishyFactory.h"
#include "System/SoftBodySystem.h"
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
		addSystem<SoftBodySystem>();
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
		auto woodTex = wf::loadTexture("resources/images/wood_planks_12_color_1k.png");
		auto woodNorm = wf::loadTexture("resources/images/wood_planks_12_normal_gl_1k.png");

		// squishies
		createSquishy("Squishy 1", { -2.f, -5.f, 0.f }, wf::RED)
			.addComponent<Component::UserControl>();
		createSquishy("Squishy 2", { -1.2f, -3.f, 0.f }, wf::BLUE);
		//createSquishy("Squishy 3", { 5.f, 5.f, 0.f }, wf::YELLOW);

		// create a temporary floor
		{
			auto obj = createObject({ 0.f, -15.f, 0.f });
			obj.addComponent<wf::component::NameTag>("Beam");
			auto& material = obj.addComponent<wf::component::Material>();
			material.diffuse.map = woodTex;
			material.normal.map = woodNorm;
			auto& beam = obj.addComponent<Component::SoftBody>(SquishyFactory::createRect(100.f, 10.f));
			beam.setFixed();
			obj.addComponent<Component::Collider>(CollisionGroup::STATIC, CollisionGroup::ALL & ~(CollisionGroup::STATIC));
		}

		// and a platform
		{
			auto platformSquishy = SquishyFactory::createRect(10.f, 2.f);
			platformSquishy.poly.rotate(-30.f);

			auto obj = createObject({ -5.f, -5.f, 0.f });
			obj.addComponent<wf::component::NameTag>("Platform");
			auto& material = obj.addComponent<wf::component::Material>();
			material.diffuse.map = woodTex;
			material.normal.map = woodNorm;
			obj.addComponent<Component::SoftBody>(platformSquishy).setFixed();
			obj.addComponent<Component::Collider>(CollisionGroup::STATIC, CollisionGroup::ALL & ~(CollisionGroup::STATIC));
		}

		{
			auto obj = createObject({ 5.f, 5.f, 0.f });
			obj.addComponent<wf::component::NameTag>("Gear");
			auto& material = obj.addComponent<wf::component::Material>();
			//material.specular.intensity = 1.5f;
			obj.addComponent<Component::SoftBody>(SquishyFactory::createGear(1.5, 10, .3f, wf::BLACK)).setFixed();
			obj.addComponent<Component::Collider>(CollisionGroup::KINEMATIC);
		}

		wf::Scene::setup();
	}

	void GameScene::renderGui(float dt)
	{
		auto& camera = *getCurrentCamera();
		auto& light = *getCurrentLight();

		ImGui::Begin("Squishies");
		{
			ImGui::Checkbox("Debug", &m_debug);

			if (ImGui::Button("Reset")) {
				resetSquishies();
			}

			ImGui::Separator();

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

			// INVENTORIES
			getEntityManager()->each<wf::component::NameTag, Component::Inventory>(
				[&](wf::EntityID id, const wf::component::NameTag& nametag, Component::Inventory& inventory) {
					ImGui::PushID(static_cast<int>(id));
					ImGui::SeparatorText(nametag.name.c_str());

					for (auto& item : inventory.items) {
						ImGui::SliderInt(Component::InventoryItem::getWeaponName(item.type).c_str(), &item.count, 0, 100);
					}

					auto sel = Component::InventoryItem::getWeaponName(inventory.items[inventory.selectedIndex].type);
					ImGui::Text("Selected: %s (%d)", sel.c_str(), (int)inventory.items[inventory.selectedIndex].type);

					ImGui::PopID();
				});

			if (ImGui::CollapsingHeader("Obj explorere")) {
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

						if (ent.hasComponent<Component::SoftBody>()) {
							auto& squishy = ent.getComponent<Component::SoftBody>();

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
		}
		ImGui::End();

		if (m_debug) {
			// any debug points we have, render now..
			wf::Debug::render();
		}
	}

	void GameScene::resetSquishies()
	{
		getEntityManager()->each<wf::component::Transform, Component::SoftBody>(
			[&](wf::EntityID id, wf::component::Transform& transform, Component::SoftBody& softbody) {
				for (size_t i = 0; i < softbody.points.size(); i++) {
					softbody.points[i].position = wf::Vec3(softbody.shape.poly.points[i], 0.f) + softbody.originalPosition;
					softbody.points[i].velocity = softbody.points[i].force = {};
				}

				softbody.updateDerivedData();
				softbody.updateEdges();
				softbody.updateBoundingBox();
			});
	}

	wf::Entity GameScene::createSquishy(const std::string& name, const wf::Vec3 pos, const wf::Colour& colour)
	{
		static Squishy _proto = SquishyFactory::createCircle(
			1.f,		// radius
			20,			// number of points on the squishies
			3			// how much support with joints etc.
		);

		Squishy proto = _proto;
		proto.colour = colour;

		// main object
		auto obj = createObject(pos);
		obj.addComponent<wf::component::NameTag>(name);
		obj.addComponent<Component::Character>();
		auto& material = obj.addComponent<wf::component::Material>();
		//material.specular.intensity = 1.5f;

		// collider
		obj.addComponent<Component::Collider>(CollisionGroup::CHARACTER);

		// main softbody instance
		obj.addComponent<Component::SoftBody>(proto);

		// inventory for weapons
		resetInventory(obj);

		return obj;
	}

	void GameScene::resetInventory(wf::Entity entity)
	{
		if (!entity.hasComponent<Component::Inventory>()) {
			entity.addComponent<Component::Inventory>();
		}

		auto& inv = entity.getComponent<Component::Inventory>();
		inv.items.clear();
		inv.selectedIndex = 0;

		inv.items.push_back({ Component::WeaponType::NADE, 5 });
		inv.items.push_back({ Component::WeaponType::MINE, 5 });
		inv.items.push_back({ Component::WeaponType::BALLOON, 5 });
	}
}
