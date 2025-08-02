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

				RenderContext ctx(&camera, scene->getCurrentLight());
				material.bind(ctx, transform);

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
			material.shader = loadBasicShader();
		}
	}
}