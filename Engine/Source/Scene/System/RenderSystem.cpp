#include "pch.h"
#include "RenderSystem.h"

#include "Scene/Component/Camera.h"
#include "Scene/Component/Geometry.h"
#include "Scene/Component/Light.h"
#include "Scene/Component/Material.h"
#include "Scene/Component/Transform.h"

namespace wf::system
{
	RenderSystem::RenderSystem(Scene* scene) : ISystem(scene)
	{
	}

	bool RenderSystem::init()
	{
		return true;
	}

	void RenderSystem::update(float dt)
	{
		// for any geometry we've not prepared, we'll need to create the VAO/VBOs for it.
		// if we already have those, and we're working with a dynamic mesh, see if it needs updating and update it
		entityManager->each<GeometryComponent>(
			[&](GeometryComponent& geometry) {
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
		entityManager->each<MaterialComponent>(
			[&](MaterialComponent& material) {
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

	void RenderSystem::render(float dt)
	{
		wgl::clearColour(scene->getBackgroundColour(), true);

		auto& camera = *scene->getCurrentCamera();

		entityManager->each<GeometryComponent, MaterialComponent, TransformComponent>(
			[&](const GeometryComponent& geometry, const MaterialComponent& material, const TransformComponent& transform) {
				if (!material.visible) return;
				if (!geometry.mesh || !geometry.mesh->handle || !m_meshBuffers.contains(geometry.mesh->handle)) return;
				if (!material.shader || !material.shader->handle) return;

				auto& buffers = m_meshBuffers[geometry.mesh->handle];
				auto& shader = material.shader;
				auto& shaderHandle = m_shaders[material.shader->handle];

				wgl::useShader(shaderHandle);

				if (material.hasDiffuseTexture()) {
					wgl::bindTexture(material.diffuse.map.get()->handle, 0);
				}
				if (material.hasNormalTexture()) {
					wgl::bindTexture(material.normal.map.get()->handle, 1);
				}
				if (material.hasSpecularTexture()) {
					wgl::bindTexture(material.specular.map.get()->handle, 2);
				}
				if (material.hasShadowMap()) {
					wgl::bindTexture(material.shadow.map->depthTexture, 3);

					wgl::setShaderUniform(
						shaderHandle,
						shader->locs["shadowMapResolution"],
						(float)material.shadow.map->width
					);

					wgl::setShaderUniform(
						shaderHandle,
						shader->locs["shadowBias"],
						.005f
					);
				}

				Mat4 matModel = transform.getTransformMatrix();
				Mat4 mvp(camera.getViewProjectionMatrix() * matModel);

				wgl::setShaderUniform(
					shaderHandle,
					shader->locs["lightVP"],
					scene->getCurrentLight()->getViewProjectionMatrix()
				);
				wgl::setShaderUniform(
					shaderHandle,
					shader->locs["lightDir"],
					scene->getCurrentLight()->getDirection()
				);

				wgl::setShaderUniform(
					shaderHandle,
					shader->locs["viewPos"],
					camera.position
				);
				wgl::setShaderUniform(
					shaderHandle,
					shader->locs["matModel"],
					matModel
				);
				wgl::setShaderUniform(
					shaderHandle,
					shader->locs["mvp"],
					mvp
				);
				wgl::setShaderUniform(
					shaderHandle,
					shader->locs["lightColour"],
					scene->getCurrentLight()->colour
				);
				wgl::setShaderUniform(
					shaderHandle,
					shader->locs["ambientLevel"],
					scene->getCurrentLight()->ambientLevel
				);

				wgl::setCullMode(material.cullMode);
				wgl::setBlendMode(material.blendMode);
				wgl::enableDepthTest(material.depthTest); // @todo apparently only supposed to be done once unless necessary according to gpt.
				wgl::enableDepthMask(material.depthMask);
				wgl::setDepthFunc(material.depthFunc);

				wgl::drawMeshBuffers(
					buffers,
					static_cast<int>(geometry.mesh->vertices.size()),
					static_cast<int>(geometry.mesh->indices.size()),
					material.wireframe
				);
			});
	}

	void RenderSystem::teardown()
	{
		// delete material (i.e. textures and shaders)
		entityManager->each<MaterialComponent>(
			[&](MaterialComponent& material) {
				if (!material.shader->handle) return;
				if (material.diffuse.map && material.diffuse.map->handle.glId) {
					wgl::destroyTexture(material.diffuse.map.get()->handle);
				}
				if (material.normal.map && material.normal.map->handle.glId) {
					wgl::destroyTexture(material.normal.map.get()->handle);
				}
				if (material.specular.map && material.specular.map->handle.glId) {
					wgl::destroyTexture(material.specular.map.get()->handle);
				}

				if (m_shaders.contains(material.shader->handle)) {
					auto& shader = m_shaders[material.shader->handle];
					wgl::destroyShader(shader);
					m_shaders.erase(material.shader->handle);
					material.shader->handle = {};
				}
			});

		// delete the VAO/VBOs
		entityManager->each<GeometryComponent>(
			[&](GeometryComponent& geometry) {
				if (!geometry.mesh->handle) return;

				if (!m_meshBuffers.contains(geometry.mesh->handle)) throw std::runtime_error("Encountered an unmanaged MeshBuffer");

				auto& buffers = m_meshBuffers[geometry.mesh->handle];
				wgl::destroyMeshBuffers(buffers);
				m_meshBuffers.erase(geometry.mesh->handle);
				geometry.mesh->handle = {};
			});

		// @todo delete FBOs, although we don't yet manage those here
	}

	void RenderSystem::uploadMesh(GeometryComponent& geometry)
	{
		auto buffers = wgl::createMeshBuffers();
		wgl::uploadMeshData(buffers, geometry.mesh->vertices, geometry.mesh->indices, geometry.mesh->isDynamic);
		auto id = ++m_lastId;
		geometry.mesh->handle = id;
		m_meshBuffers[id] = buffers;
	}

	void RenderSystem::updateMeshData(const GeometryComponent& geometry)
	{
		if (geometry.mesh->isDynamic && (geometry.mesh->needsUpdate || geometry.mesh->autoUpdate)) {
			auto& buffers = m_meshBuffers[geometry.mesh->handle];
			wgl::updateMeshData(buffers, geometry.mesh->vertices, geometry.mesh->indices);
		}
	}

	void RenderSystem::uploadMaterialData(MaterialComponent& material)
	{
		if (!material.shader) {
			material.shader = Shader::create();
		}

		if (!material.shader->handle) {
			auto shader = wgl::loadShader("resources/shaders/phong.vert", "resources/shaders/phong.frag");
			auto id = ++m_lastId;
			material.shader->handle = id;
			m_shaders[id] = shader;

			// transform
			material.shader->locs["mvp"] = wgl::getShaderUniformLocation(shader, "mvp");
			material.shader->locs["matModel"] = wgl::getShaderUniformLocation(shader, "matModel");

			// textures
			material.shader->locs["diffuseMap"] = wgl::getShaderUniformLocation(shader, "diffuseMap");
			material.shader->locs["normalMap"] = wgl::getShaderUniformLocation(shader, "normalMap");
			material.shader->locs["specularMap"] = wgl::getShaderUniformLocation(shader, "specularMap");
			material.shader->locs["shadowMap"] = wgl::getShaderUniformLocation(shader, "shadowMap");
			// flags
			material.shader->locs["hasDiffuseMap"] = wgl::getShaderUniformLocation(shader, "hasDiffuseMap");
			material.shader->locs["hasNormalMap"] = wgl::getShaderUniformLocation(shader, "hasNormalMap");
			material.shader->locs["hasSpecularMap"] = wgl::getShaderUniformLocation(shader, "hasSpecularMap");
			material.shader->locs["hasShadowMap"] = wgl::getShaderUniformLocation(shader, "hasShadowMap");
			// material props
			material.shader->locs["diffuseColour"] = wgl::getShaderUniformLocation(shader, "diffuseColour");
			material.shader->locs["specularColour"] = wgl::getShaderUniformLocation(shader, "specularColour");
			material.shader->locs["normalStrength"] = wgl::getShaderUniformLocation(shader, "normalStrength");
			material.shader->locs["specularShininess"] = wgl::getShaderUniformLocation(shader, "specularShininess");
			material.shader->locs["specularIntensity"] = wgl::getShaderUniformLocation(shader, "specularIntensity");
			// lights & camera
			material.shader->locs["viewPos"] = wgl::getShaderUniformLocation(shader, "viewPos");
			material.shader->locs["lightDir"] = wgl::getShaderUniformLocation(shader, "lightDir");
			material.shader->locs["lightColour"] = wgl::getShaderUniformLocation(shader, "lightColour");
			material.shader->locs["ambientLevel"] = wgl::getShaderUniformLocation(shader, "ambientLevel");
			// shadows
			material.shader->locs["lightVP"] = wgl::getShaderUniformLocation(shader, "lightVP");
			material.shader->locs["shadowBias"] = wgl::getShaderUniformLocation(shader, "shadowBias");
			material.shader->locs["shadowMapResolution"] = wgl::getShaderUniformLocation(shader, "shadowMapResolution");
		}
	}

	void RenderSystem::updateMaterialData(const MaterialComponent& material)
	{
		if (!material.shader || !material.shader->handle) throw std::runtime_error("NO shader loaded");

		auto& shader = material.shader;
		auto& shaderHandle = m_shaders[shader->handle];

		wgl::useShader(shaderHandle);

		// DIFFUSE
		if (shader->isValidLocation("hasDiffuseMap") && shader->isValidLocation("diffuseMap")) {
			wgl::setShaderUniform(shaderHandle, shader->locs["hasDiffuseMap"], material.hasDiffuseTexture());

			if (material.hasDiffuseTexture()) {
				wgl::setShaderUniform(
					shaderHandle,
					shader->locs["diffuseMap"],
					0
				);
			}
		}

		if (shader->isValidLocation("diffuseColour")) {
			wgl::setShaderUniform(
				shaderHandle,
				shader->locs["diffuseColour"],
				material.diffuse.colour
			);
		}

		// NORMAL
		if (shader->isValidLocation("hasNormalMap") && shader->isValidLocation("normalMap")) {
			wgl::setShaderUniform(shaderHandle, shader->locs["hasNormalMap"], material.hasNormalTexture());

			if (material.hasNormalTexture()) {
				wgl::setShaderUniform(
					shaderHandle,
					shader->locs["normalMap"],
					1
				);
			}

			if (shader->isValidLocation("normalStrength")) {
				wgl::setShaderUniform(
					shaderHandle,
					shader->locs["normalStrength"],
					material.normal.strength
				);
			}
		}

		// SPECULAR
		if (shader->isValidLocation("hasSpecularMap") && shader->isValidLocation("specularMap")) {
			wgl::setShaderUniform(shaderHandle, shader->locs["hasSpecularMap"], material.hasSpecularTexture());

			if (material.hasSpecularTexture()) {
				wgl::setShaderUniform(
					shaderHandle,
					shader->locs["specularMap"],
					2
				);
			}
		}

		// SHADOW
		if (shader->isValidLocation("hasShadowMap") && shader->isValidLocation("shadowMap")) {
			wgl::setShaderUniform(shaderHandle, shader->locs["hasShadowMap"], material.hasShadowMap());

			wgl::setShaderUniform(
				shaderHandle,
				shader->locs["shadowMap"],
				3
			);
		}

		if (shader->isValidLocation("specularColour")) {
			wgl::setShaderUniform(
				shaderHandle,
				shader->locs["specularColour"],
				material.specular.colour
			);
		}
		if (shader->isValidLocation("specularShininess")) {
			wgl::setShaderUniform(
				shaderHandle,
				shader->locs["specularShininess"],
				material.specular.shininess
			);
		}

		if (shader->isValidLocation("specularIntensity")) {
			wgl::setShaderUniform(
				shaderHandle,
				shader->locs["specularIntensity"],
				material.specular.intensity
			);
		}
	}
}