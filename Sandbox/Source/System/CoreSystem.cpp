#include "CoreSystem.h"

#include "Component/Geometry.h"
#include "Component/Material.h"
#include "Component/Transform.h"

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
				if (!material.handle) {

				}

				// set the uniforms

			});

		// @todo framebuffers/render targets
		// @todo textures as standalone? or are they always associated with material?
		// @todo for the above, perhaps we need to attach an internal component to things we've done for speed, but that can wait...
	}

	void CoreSystem::render(float dt)
	{
		// @todo render meshes with assigned material
		entityManager->each<Component::Geometry, Component::Material, Component::Transform>(
			[&](const Component::Geometry& geometry, const Component::Material& material, const Component::Transform& transform) {
				if (!geometry.mesh || !geometry.mesh->handle || !m_meshBuffers.contains(geometry.mesh->handle)) return;

				auto& buffers = m_meshBuffers[geometry.mesh->handle];

				// @todo from the material, we need to bind the textures & shaders. uniforms will have already been set by the update()

				wf::wgl::drawMeshBuffers(buffers, geometry.mesh->vertices.size(), geometry.mesh->indices.size(), geometry.mesh->wireframe);
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
				// @todo
			});

		// delete the VAO/VBOs
		entityManager->each<Component::Geometry>(
			[&](Component::Geometry& geometry) {
				if (!geometry.mesh->handle) return;

				if (!m_meshBuffers.contains(geometry.mesh->handle)) throw std::runtime_error("Encounted an unmanaged MeshBuffer");

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
}