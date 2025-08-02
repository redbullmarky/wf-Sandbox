#include "pch.h"
#include "RenderSystem.h"

#include "Core/Core.h"
#include "Render/Render.h"
#include "Scene/Component/CameraComponent.h"
#include "Scene/Component/LightComponent.h"
#include "Scene/Component/MeshRendererComponent.h"
#include "Scene/Component/TransformComponent.h"

namespace wf::system
{
	void RenderSystem::update(float dt)
	{
		// for any geometry we've not prepared, we'll need to create the VAO/VBOs for it.
		// if we already have those, and we're working with a dynamic mesh, see if it needs updating and update it
		entityManager->each<MeshRendererComponent>(
			[&](MeshRendererComponent& meshRenderer) {
				// no mesh yet
				if (!meshRenderer.mesh) return;

				// create the VAO/VBOs else update them if necessary
				if (!meshRenderer.mesh->buffers.vao) {
					if (meshRenderer.mesh->vertices.size()) {
						meshRenderer.mesh->buffers = wgl::createMeshBuffers();

						wgl::uploadMeshData(meshRenderer.mesh->buffers, meshRenderer.mesh->vertices, meshRenderer.mesh->indices, meshRenderer.mesh->isDynamic);
					}
				}
				else {
					// update the mesh data
					if (meshRenderer.mesh->isDynamic && (meshRenderer.mesh->needsUpdate || meshRenderer.mesh->autoUpdate)) {
						wgl::updateMeshData(meshRenderer.mesh->buffers, meshRenderer.mesh->vertices, meshRenderer.mesh->indices);
					}
				}
			});

		// for any materials we've not prepared, we'll setup the textures/shaders, etc.
		// for those we have, we'll update the shader uniforms
		entityManager->each<MeshRendererComponent>(
			[&](MeshRendererComponent& meshRenderer) {
				// if we've not got a handle, prep it.
				if (!meshRenderer.material.shader.handle.glId) {
					if (!meshRenderer.material.shader.handle.glId) {
						meshRenderer.material.shader = loadBasicShader();
					}
				}
			});

		// @todo framebuffers/render targets
		// @todo textures as standalone? or are they always associated with material?
		// @todo for the above, perhaps we need to attach an internal component to things we've done for speed, but that can wait...
	}

	void RenderSystem::render(float dt)
	{
		wgl::clearColour(scene->getBackgroundColour(), true);

		entityManager->each<MeshRendererComponent, TransformComponent>(
			[&](const MeshRendererComponent& meshRenderer, const TransformComponent& transform) {

				auto& material = meshRenderer.material;

				if (!material.visible) return;
				if (!meshRenderer.mesh || !meshRenderer.mesh->buffers.vao) return;
				if (!material.shader.handle.glId) return;

				RenderContext ctx(scene->getCurrentCamera(), scene->getCurrentLight());
				material.bind(ctx, transform);

				wgl::drawMeshBuffers(
					meshRenderer.mesh->buffers,
					static_cast<int>(meshRenderer.mesh->vertices.size()),
					static_cast<int>(meshRenderer.mesh->indices.size()),
					material.wireframe
				);
			});
	}

	void RenderSystem::teardown()
	{
		entityManager->each<MeshRendererComponent>(
			[&](MeshRendererComponent& meshRenderer) {

				// delete material (i.e. textures and shaders)
				if (meshRenderer.material.shader.handle.glId) {
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
				}

				// delete the VAO/VBOs
				if (meshRenderer.mesh->buffers.vao) {
					wgl::destroyMeshBuffers(meshRenderer.mesh->buffers);
					meshRenderer.mesh->buffers = {};
				}
			});
	}
}