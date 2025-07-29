#include "CoreSystem.h"
#include "Engine.h"

#include "Component/Geometry.h"
#include "Component/Material.h"

#include "Scene/GameScene.h"

#include <GL/glew.h>

namespace Sandbox
{
	CoreSystem::CoreSystem(wf::Scene* scene) : wf::ISystem(scene)
	{
	}

	bool CoreSystem::init()
	{
		return true;
	}

	void CoreSystem::update(float dt)
	{
		// for any geometry we've not prepared, we'll need to create the VAO/VBOs for it.
		// if we already have those, and we're working with a dynamic mesh, see if it needs updating and update it
		entityManager->each<Component::Geometry>(
			[&](Component::Geometry& geometry) {
				// no mesh yet
				if (!geometry.mesh) return;

				// create the VAO/VBOs else update them if necessary
				if (!geometry.mesh->handle) {
					if (geometry.mesh->vertices.size()) {
						uploadMesh(geometry);
					}
				}
				else {
					updateMeshData(geometry);
				}
			});

		// for any materials we've not prepared, we'll setup the textures/shaders, etc.
		// for those we have, we'll update the shader uniforms
		entityManager->each<Component::Material>(
			[&](Component::Material& material) {
				// if we've not got a handle, prep it.
				if (!material.shader || !material.shader->handle) {
					uploadMaterialData(material);
				}

				// set the uniforms
				updateMaterialData(material);
			});

		// @todo framebuffers/render targets
		// @todo textures as standalone? or are they always associated with material?
		// @todo for the above, perhaps we need to attach an internal component to things we've done for speed, but that can wait...
	}

	void CoreSystem::render(float dt)
	{
		wf::wgl::clearColour(scene->getBackgroundColour(), true);

		auto& camera = *scene->getCurrentCamera();

		/*GameScene* gameScene = static_cast<GameScene*>(scene);
		auto& camera = gameScene->m_lightCamera;*/

		// @todo render meshes with assigned material
		entityManager->each<Component::Geometry, Component::Material, wf::component::Transform>(
			[&](const Component::Geometry& geometry, const Component::Material& material, const wf::component::Transform& transform) {
				if (!geometry.mesh || !geometry.mesh->handle || !m_meshBuffers.contains(geometry.mesh->handle)) return;
				if (!material.shader || !material.shader->handle) return;

				auto& buffers = m_meshBuffers[geometry.mesh->handle];
				auto& shader = material.shader;
				auto& shaderHandle = m_shaders[material.shader->handle];

				wf::wgl::useShader(shaderHandle);

				if (material.hasDiffuseTexture()) {
					wf::wgl::bindTexture(material.diffuse.map.get()->handle, 0);
				}
				if (material.hasNormalTexture()) {
					wf::wgl::bindTexture(material.normal.map.get()->handle, 1);
				}
				if (material.hasSpecularTexture()) {
					wf::wgl::bindTexture(material.specular.map.get()->handle, 2);
				}
				if (material.hasShadowMap()) {
					wf::wgl::bindTexture(material.shadow.map->depthTexture, 3);
				}

				wf::Mat4 matModel = transform.getTransformMatrix();
				wf::Mat4 mvp(camera.getViewProjectionMatrix() * matModel);

				wf::wgl::setShaderUniform(
					shaderHandle,
					shader->locs["lightVP"],
					scene->getCurrentLight()->getViewProjectionMatrix()
				);
				wf::wgl::setShaderUniform(
					shaderHandle,
					shader->locs["lightDir"],
					scene->getCurrentLight()->getDirection()
				);

				wf::wgl::setShaderUniform(
					shaderHandle,
					shader->locs["viewPos"],
					camera.position
				);
				wf::wgl::setShaderUniform(
					shaderHandle,
					shader->locs["matModel"],
					matModel
				);
				wf::wgl::setShaderUniform(
					shaderHandle,
					shader->locs["mvp"],
					mvp
				);
				wf::wgl::setShaderUniform(
					shaderHandle,
					shader->locs["lightColour"],
					scene->getCurrentLight()->colour
				);
				wf::wgl::setShaderUniform(
					shaderHandle,
					shader->locs["ambientLevel"],
					scene->getCurrentLight()->ambientLevel
				);

				// @todo we'll need to shifty this gl stuff out of here...
				switch (material.cullMode)
				{
				case  Component::Material::CullMode::NONE:
					glDisable(GL_CULL_FACE);
					break;
				case Component::Material::CullMode::BACK:
					glEnable(GL_CULL_FACE);
					glCullFace(GL_BACK);
					break;
				case Component::Material::CullMode::FRONT:
					glEnable(GL_CULL_FACE);
					glCullFace(GL_FRONT);
					break;
				}

				switch (material.blendMode)
				{
				case Component::Material::BlendMode::OPAQUE:
					glDisable(GL_BLEND);
					break;
				case Component::Material::BlendMode::ALPHA:
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					break;
				case Component::Material::BlendMode::ADDITIVE:
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE);
					break;
				case Component::Material::BlendMode::MULTIPLY:
					glEnable(GL_BLEND);
					glBlendFunc(GL_DST_COLOR, GL_ZERO);
					break;
				}

				// @todo apparently only supposed to be done once unless necessary according to gpt.
				wf::wgl::enableDepthTest(material.depthTest);
				wf::wgl::enableDepthMask(material.depthMask);

				// @todo depth func? if we find out what it's for...

				wf::wgl::drawMeshBuffers(
					buffers,
					static_cast<int>(geometry.mesh->vertices.size()),
					static_cast<int>(geometry.mesh->indices.size()),
					geometry.mesh->wireframe
				);
			});

		// @todo sprites
		// @todo gui (frontend)
		// @todo imgui
	}

	void CoreSystem::teardown()
	{
		// delete material (i.e. textures and shaders)
		entityManager->each<Component::Material>(
			[&](Component::Material& material) {
				if (!material.shader->handle) return;
				if (material.diffuse.map && material.diffuse.map->handle.glId) {
					wf::wgl::destroyTexture(material.diffuse.map.get()->handle);
				}
				if (material.normal.map && material.normal.map->handle.glId) {
					wf::wgl::destroyTexture(material.normal.map.get()->handle);
				}
				if (material.specular.map && material.specular.map->handle.glId) {
					wf::wgl::destroyTexture(material.specular.map.get()->handle);
				}

				if (m_shaders.contains(material.shader->handle)) {
					auto& shader = m_shaders[material.shader->handle];
					wf::wgl::destroyShader(shader);
					m_shaders.erase(material.shader->handle);
					material.shader->handle = {};
				}
			});

		// delete the VAO/VBOs
		entityManager->each<Component::Geometry>(
			[&](Component::Geometry& geometry) {
				if (!geometry.mesh->handle) return;

				if (!m_meshBuffers.contains(geometry.mesh->handle)) throw std::runtime_error("Encountered an unmanaged MeshBuffer");

				auto& buffers = m_meshBuffers[geometry.mesh->handle];
				wf::wgl::destroyMeshBuffers(buffers);
				m_meshBuffers.erase(geometry.mesh->handle);
				geometry.mesh->handle = {};
			});

		// @todo delete FBOs
	}

	void CoreSystem::uploadMesh(Component::Geometry& geometry)
	{
		auto buffers = wf::wgl::createMeshBuffers();
		wf::wgl::uploadMeshData(buffers, geometry.mesh->vertices, geometry.mesh->indices, geometry.mesh->isDynamic);
		auto id = ++m_lastId;
		geometry.mesh->handle = id;
		m_meshBuffers[id] = buffers;
	}

	void CoreSystem::updateMeshData(const Component::Geometry& geometry)
	{
		if (geometry.mesh->isDynamic && (geometry.mesh->needsUpdate || geometry.mesh->autoUpdate)) {
			auto& buffers = m_meshBuffers[geometry.mesh->handle];
			wf::wgl::updateMeshData(buffers, geometry.mesh->vertices, geometry.mesh->indices);
		}
	}

	void CoreSystem::uploadMaterialData(Component::Material& material)
	{
		if (!material.shader) {
			material.shader = wf::Shader::create();
		}

		if (!material.shader->handle) {
			auto shader = wf::wgl::loadShader("resources/shaders/phong.vert", "resources/shaders/phong.frag");
			auto id = ++m_lastId;
			material.shader->handle = id;
			m_shaders[id] = shader;

			// transform
			material.shader->locs["mvp"] = wf::wgl::getShaderUniformLocation(shader, "mvp");
			material.shader->locs["matModel"] = wf::wgl::getShaderUniformLocation(shader, "matModel");

			// textures
			material.shader->locs["diffuseMap"] = wf::wgl::getShaderUniformLocation(shader, "diffuseMap");
			material.shader->locs["normalMap"] = wf::wgl::getShaderUniformLocation(shader, "normalMap");
			material.shader->locs["specularMap"] = wf::wgl::getShaderUniformLocation(shader, "specularMap");
			material.shader->locs["shadowMap"] = wf::wgl::getShaderUniformLocation(shader, "shadowMap");
			// flags
			material.shader->locs["hasDiffuseMap"] = wf::wgl::getShaderUniformLocation(shader, "hasDiffuseMap");
			material.shader->locs["hasNormalMap"] = wf::wgl::getShaderUniformLocation(shader, "hasNormalMap");
			material.shader->locs["hasSpecularMap"] = wf::wgl::getShaderUniformLocation(shader, "hasSpecularMap");
			material.shader->locs["hasShadowMap"] = wf::wgl::getShaderUniformLocation(shader, "hasShadowMap");
			// material props
			material.shader->locs["diffuseColour"] = wf::wgl::getShaderUniformLocation(shader, "diffuseColour");
			material.shader->locs["specularColour"] = wf::wgl::getShaderUniformLocation(shader, "specularColour");
			material.shader->locs["normalStrength"] = wf::wgl::getShaderUniformLocation(shader, "normalStrength");
			material.shader->locs["specularShininess"] = wf::wgl::getShaderUniformLocation(shader, "specularShininess");
			material.shader->locs["specularIntensity"] = wf::wgl::getShaderUniformLocation(shader, "specularIntensity");
			// lights & camera
			material.shader->locs["viewPos"] = wf::wgl::getShaderUniformLocation(shader, "viewPos");
			material.shader->locs["lightVP"] = wf::wgl::getShaderUniformLocation(shader, "lightVP");
			material.shader->locs["lightDir"] = wf::wgl::getShaderUniformLocation(shader, "lightDir");
			material.shader->locs["lightColour"] = wf::wgl::getShaderUniformLocation(shader, "lightColour");
			material.shader->locs["ambientLevel"] = wf::wgl::getShaderUniformLocation(shader, "ambientLevel");
		}
	}

	void CoreSystem::updateMaterialData(const Component::Material& material)
	{
		if (!material.shader || !material.shader->handle) throw std::runtime_error("NO shader loaded");

		auto& shader = material.shader;
		auto& shaderHandle = m_shaders[shader->handle];

		wf::wgl::useShader(shaderHandle);

		// DIFFUSE
		if (shader->isValidLocation("hasDiffuseMap") && shader->isValidLocation("diffuseMap")) {
			wf::wgl::setShaderUniform(shaderHandle, shader->locs["hasDiffuseMap"], material.hasDiffuseTexture());

			if (material.hasDiffuseTexture()) {
				wf::wgl::setShaderUniform(
					shaderHandle,
					shader->locs["diffuseMap"],
					0
				);
			}
		}

		if (shader->isValidLocation("diffuseColour")) {
			wf::wgl::setShaderUniform(
				shaderHandle,
				shader->locs["diffuseColour"],
				material.diffuse.colour
			);
		}

		// NORMAL
		if (shader->isValidLocation("hasNormalMap") && shader->isValidLocation("normalMap")) {
			wf::wgl::setShaderUniform(shaderHandle, shader->locs["hasNormalMap"], material.hasNormalTexture());

			if (material.hasNormalTexture()) {
				wf::wgl::setShaderUniform(
					shaderHandle,
					shader->locs["normalMap"],
					1
				);
			}

			if (shader->isValidLocation("normalStrength")) {
				wf::wgl::setShaderUniform(
					shaderHandle,
					shader->locs["normalStrength"],
					material.normal.strength
				);
			}
		}

		// SPECULAR
		if (shader->isValidLocation("hasSpecularMap") && shader->isValidLocation("specularMap")) {
			wf::wgl::setShaderUniform(shaderHandle, shader->locs["hasSpecularMap"], material.hasSpecularTexture());

			if (material.hasSpecularTexture()) {
				wf::wgl::setShaderUniform(
					shaderHandle,
					shader->locs["specularMap"],
					2
				);
			}
		}

		// SHADOW
		if (shader->isValidLocation("hasShadowMap") && shader->isValidLocation("shadowMap")) {
			wf::wgl::setShaderUniform(shaderHandle, shader->locs["hasShadowMap"], material.hasShadowMap());

			wf::wgl::setShaderUniform(
				shaderHandle,
				shader->locs["shadowMap"],
				3
			);
		}

		if (shader->isValidLocation("specularColour")) {
			wf::wgl::setShaderUniform(
				shaderHandle,
				shader->locs["specularColour"],
				material.specular.colour
			);
		}
		if (shader->isValidLocation("specularShininess")) {
			wf::wgl::setShaderUniform(
				shaderHandle,
				shader->locs["specularShininess"],
				material.specular.shininess
			);
		}

		if (shader->isValidLocation("specularIntensity")) {
			wf::wgl::setShaderUniform(
				shaderHandle,
				shader->locs["specularIntensity"],
				material.specular.intensity
			);
		}
	}
}