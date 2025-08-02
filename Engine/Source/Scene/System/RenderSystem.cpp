#include "pch.h"
#include "RenderSystem.h"

#include "Render/Render.h"
#include "Scene/Component/CameraComponent.h"
#include "Scene/Component/GeometryComponent.h"
#include "Scene/Component/LightComponent.h"
#include "Scene/Component/MeshRendererComponent.h"
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
		entityManager->each<MeshRendererComponent>(
			[&](MeshRendererComponent& meshRenderer) {
				// if we've not got a handle, prep it.
				if (!meshRenderer.material.shader.handle.glId) {
					uploadMaterialData(meshRenderer.material);
				}

				// set the uniforms
				updateMaterialData(meshRenderer.material);
			});

		// @todo framebuffers/render targets
		// @todo textures as standalone? or are they always associated with material?
		// @todo for the above, perhaps we need to attach an internal component to things we've done for speed, but that can wait...
	}

	void RenderSystem::render(float dt)
	{
		wgl::clearColour(scene->getBackgroundColour(), true);

		auto& camera = *scene->getCurrentCamera();

		entityManager->each<GeometryComponent, MeshRendererComponent, TransformComponent>(
			[&](const GeometryComponent& geometry, const MeshRendererComponent& meshRenderer, const TransformComponent& transform) {

				auto& material = meshRenderer.material;

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
		entityManager->each<MeshRendererComponent>(
			[&](MeshRendererComponent& meshRenderer) {
				if (!meshRenderer.material.shader.handle.glId) return;
				if (meshRenderer.material.diffuse.map.handle.glId) {
					wgl::destroyTexture(meshRenderer.material.diffuse.map.handle);
				}
				if (meshRenderer.material.normal.map.handle.glId) {
					wgl::destroyTexture(meshRenderer.material.normal.map.handle);
				}
				if (meshRenderer.material.specular.map.handle.glId) {
					wgl::destroyTexture(meshRenderer.material.specular.map.handle);
				}

				wgl::destroyShader(meshRenderer.material.shader.handle);
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

	void RenderSystem::uploadMaterialData(Material& material)
	{
		if (!material.shader.handle.glId) {
			material.shader = loadPhongShader();
		}
	}

	void RenderSystem::updateMaterialData(const Material& material)
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