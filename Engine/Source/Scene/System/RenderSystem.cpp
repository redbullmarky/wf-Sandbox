#include "pch.h"
#include "RenderSystem.h"

#include "Scene/Component/CameraComponent.h"
#include "Scene/Component/GeometryComponent.h"
#include "Scene/Component/LightComponent.h"
#include "Scene/Component/MaterialComponent.h"
#include "Scene/Component/TransformComponent.h"

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
				if (!geometry.mesh->buffers.vao) {
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
				if (!material.shader.handle.glId) {
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
				if (!geometry.mesh || !geometry.mesh->buffers.vao) return;
				if (!material.shader.handle.glId) return;

				auto& shader = material.shader;

				wgl::useShader(shader.handle);

				if (material.hasDiffuseTexture()) {
					wgl::bindTexture(material.diffuse.map.handle, 0);
				}
				if (material.hasNormalTexture()) {
					wgl::bindTexture(material.normal.map.handle, 1);
				}
				if (material.hasSpecularTexture()) {
					wgl::bindTexture(material.specular.map.handle, 2);
				}
				if (material.hasShadowMap()) {
					wgl::bindTexture(material.shadow.map->depthTexture, 3);

					wgl::setShaderUniform(
						shader.handle,
						shader.location("shadowMapResolution"),
						(float)material.shadow.map->width
					);

					wgl::setShaderUniform(
						shader.handle,
						shader.location("shadowBias"),
						.005f
					);
				}

				Mat4 matModel = transform.getTransformMatrix();
				Mat4 mvp(camera.getViewProjectionMatrix() * matModel);

				wgl::setShaderUniform(
					shader.handle,
					shader.location("lightVP"),
					scene->getCurrentLight()->getViewProjectionMatrix()
				);
				wgl::setShaderUniform(
					shader.handle,
					shader.location("lightDir"),
					scene->getCurrentLight()->getDirection()
				);

				wgl::setShaderUniform(
					shader.handle,
					shader.location("viewPos"),
					camera.position
				);
				wgl::setShaderUniform(
					shader.handle,
					shader.location("matModel"),
					matModel
				);
				wgl::setShaderUniform(
					shader.handle,
					shader.location("mvp"),
					mvp
				);
				wgl::setShaderUniform(
					shader.handle,
					shader.location("lightColour"),
					scene->getCurrentLight()->colour
				);
				wgl::setShaderUniform(
					shader.handle,
					shader.location("ambientLevel"),
					scene->getCurrentLight()->ambientLevel
				);

				wgl::setCullMode(material.cullMode);
				wgl::setBlendMode(material.blendMode);
				wgl::enableDepthTest(material.depthTest); // @todo apparently only supposed to be done once unless necessary according to gpt.
				wgl::enableDepthMask(material.depthMask);
				wgl::setDepthFunc(material.depthFunc);

				wgl::drawMeshBuffers(
					geometry.mesh->buffers,
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
				if (!material.shader.handle.glId) return;
				if (material.diffuse.map.handle.glId) {
					wgl::destroyTexture(material.diffuse.map.handle);
				}
				if (material.normal.map.handle.glId) {
					wgl::destroyTexture(material.normal.map.handle);
				}
				if (material.specular.map.handle.glId) {
					wgl::destroyTexture(material.specular.map.handle);
				}

				wgl::destroyShader(material.shader.handle);
			});

		// delete the VAO/VBOs
		entityManager->each<GeometryComponent>(
			[&](GeometryComponent& geometry) {
				if (!geometry.mesh->buffers.vao) return;

				wgl::destroyMeshBuffers(geometry.mesh->buffers);
				geometry.mesh->buffers = {};
			});

		// @todo delete FBOs, although we don't yet manage those here
	}

	void RenderSystem::uploadMesh(GeometryComponent& geometry)
	{
		geometry.mesh->buffers = wgl::createMeshBuffers();
		wgl::uploadMeshData(geometry.mesh->buffers, geometry.mesh->vertices, geometry.mesh->indices, geometry.mesh->isDynamic);
	}

	void RenderSystem::updateMeshData(const GeometryComponent& geometry)
	{
		if (geometry.mesh->isDynamic && (geometry.mesh->needsUpdate || geometry.mesh->autoUpdate)) {
			wgl::updateMeshData(geometry.mesh->buffers, geometry.mesh->vertices, geometry.mesh->indices);
		}
	}

	void RenderSystem::uploadMaterialData(MaterialComponent& material)
	{
		if (!material.shader.handle.glId) {
			material.shader.handle = wgl::loadShader("resources/shaders/phong.vert", "resources/shaders/phong.frag");

			auto shaderHandle = material.shader.handle;

			// transform
			material.shader.locs["mvp"] = wgl::getShaderUniformLocation(shaderHandle, "mvp");
			material.shader.locs["matModel"] = wgl::getShaderUniformLocation(shaderHandle, "matModel");

			// textures
			material.shader.locs["diffuseMap"] = wgl::getShaderUniformLocation(shaderHandle, "diffuseMap");
			material.shader.locs["normalMap"] = wgl::getShaderUniformLocation(shaderHandle, "normalMap");
			material.shader.locs["specularMap"] = wgl::getShaderUniformLocation(shaderHandle, "specularMap");
			material.shader.locs["shadowMap"] = wgl::getShaderUniformLocation(shaderHandle, "shadowMap");
			// flags
			material.shader.locs["hasDiffuseMap"] = wgl::getShaderUniformLocation(shaderHandle, "hasDiffuseMap");
			material.shader.locs["hasNormalMap"] = wgl::getShaderUniformLocation(shaderHandle, "hasNormalMap");
			material.shader.locs["hasSpecularMap"] = wgl::getShaderUniformLocation(shaderHandle, "hasSpecularMap");
			material.shader.locs["hasShadowMap"] = wgl::getShaderUniformLocation(shaderHandle, "hasShadowMap");
			// material props
			material.shader.locs["diffuseColour"] = wgl::getShaderUniformLocation(shaderHandle, "diffuseColour");
			material.shader.locs["specularColour"] = wgl::getShaderUniformLocation(shaderHandle, "specularColour");
			material.shader.locs["normalStrength"] = wgl::getShaderUniformLocation(shaderHandle, "normalStrength");
			material.shader.locs["specularShininess"] = wgl::getShaderUniformLocation(shaderHandle, "specularShininess");
			material.shader.locs["specularIntensity"] = wgl::getShaderUniformLocation(shaderHandle, "specularIntensity");
			// lights & camera
			material.shader.locs["viewPos"] = wgl::getShaderUniformLocation(shaderHandle, "viewPos");
			material.shader.locs["lightDir"] = wgl::getShaderUniformLocation(shaderHandle, "lightDir");
			material.shader.locs["lightColour"] = wgl::getShaderUniformLocation(shaderHandle, "lightColour");
			material.shader.locs["ambientLevel"] = wgl::getShaderUniformLocation(shaderHandle, "ambientLevel");
			// shadows
			material.shader.locs["lightVP"] = wgl::getShaderUniformLocation(shaderHandle, "lightVP");
			material.shader.locs["shadowBias"] = wgl::getShaderUniformLocation(shaderHandle, "shadowBias");
			material.shader.locs["shadowMapResolution"] = wgl::getShaderUniformLocation(shaderHandle, "shadowMapResolution");
		}
	}

	void RenderSystem::updateMaterialData(const MaterialComponent& material)
	{
		if (!material.shader.handle.glId) throw std::runtime_error("NO shader loaded");

		auto& shader = material.shader;

		wgl::useShader(material.shader.handle);

		// DIFFUSE
		if (shader.isValidLocation("hasDiffuseMap") && shader.isValidLocation("diffuseMap")) {
			wgl::setShaderUniform(material.shader.handle, shader.location("hasDiffuseMap"), material.hasDiffuseTexture());

			if (material.hasDiffuseTexture()) {
				wgl::setShaderUniform(
					material.shader.handle,
					shader.location("diffuseMap"),
					0
				);
			}
		}

		if (shader.isValidLocation("diffuseColour")) {
			wgl::setShaderUniform(
				material.shader.handle,
				shader.location("diffuseColour"),
				material.diffuse.colour
			);
		}

		// NORMAL
		if (shader.isValidLocation("hasNormalMap") && shader.isValidLocation("normalMap")) {
			wgl::setShaderUniform(material.shader.handle, shader.location("hasNormalMap"), material.hasNormalTexture());

			if (material.hasNormalTexture()) {
				wgl::setShaderUniform(
					material.shader.handle,
					shader.location("normalMap"),
					1
				);
			}

			if (shader.isValidLocation("normalStrength")) {
				wgl::setShaderUniform(
					material.shader.handle,
					shader.location("normalStrength"),
					material.normal.strength
				);
			}
		}

		// SPECULAR
		if (shader.isValidLocation("hasSpecularMap") && shader.isValidLocation("specularMap")) {
			wgl::setShaderUniform(material.shader.handle, shader.location("hasSpecularMap"), material.hasSpecularTexture());

			if (material.hasSpecularTexture()) {
				wgl::setShaderUniform(
					material.shader.handle,
					shader.location("specularMap"),
					2
				);
			}
		}

		// SHADOW
		if (shader.isValidLocation("hasShadowMap") && shader.isValidLocation("shadowMap")) {
			wgl::setShaderUniform(material.shader.handle, shader.location("hasShadowMap"), material.hasShadowMap());

			wgl::setShaderUniform(
				material.shader.handle,
				shader.location("shadowMap"),
				3
			);
		}

		if (shader.isValidLocation("specularColour")) {
			wgl::setShaderUniform(
				material.shader.handle,
				shader.location("specularColour"),
				material.specular.colour
			);
		}
		if (shader.isValidLocation("specularShininess")) {
			wgl::setShaderUniform(
				material.shader.handle,
				shader.location("specularShininess"),
				material.specular.shininess
			);
		}

		if (shader.isValidLocation("specularIntensity")) {
			wgl::setShaderUniform(
				material.shader.handle,
				shader.location("specularIntensity"),
				material.specular.intensity
			);
		}
	}
}