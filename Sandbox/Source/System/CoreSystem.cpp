#include "CoreSystem.h"

#include "Component/Geometry.h"
#include "Component/Material.h"

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
		wf::getWindow().clear(scene->getBackgroundColour());

		auto& camera = *scene->getCurrentCamera();

		// @todo render meshes with assigned material
		entityManager->each<Component::Geometry, Component::Material, wf::component::Transform>(
			[&](const Component::Geometry& geometry, const Component::Material& material, const wf::component::Transform& transform) {
				if (!geometry.mesh || !geometry.mesh->handle || !m_meshBuffers.contains(geometry.mesh->handle)) return;
				if (!material.shader || !material.shader->handle) return;

				auto& buffers = m_meshBuffers[geometry.mesh->handle];
				auto& shader = material.shader;
				auto shaderHandle = m_shaders[material.shader->handle];

				wf::wgl::useShader(shaderHandle);

				// @todo textures
				wf::Vec3 lightDir = wf::Vec3{ 0.f, -1.f, -1.f };
				wf::Vec3 viewPos = wf::Vec3{ 1.f, 0.f, 5.f };
				wf::Mat4 vp = wf::Mat4{ 1.f };
				wf::Mat4 mvp(vp * transform.getTransformMatrix());

				mvp = wf::Mat4{ 1.f };

				wf::wgl::setShaderUniform(
					shaderHandle,
					shader->locs["lightDir"],
					lightDir
				);
				wf::wgl::setShaderUniform(
					shaderHandle,
					shader->locs["viewPos"],
					viewPos
				);
				wf::wgl::setShaderUniform(
					shaderHandle,
					shader->locs["matModel"],
					wf::Mat4{}// transform.getTransformMatrix()
				);
				wf::wgl::setShaderUniform(
					shaderHandle,
					shader->locs["mvp"],
					mvp
				);
				wf::wgl::setShaderUniform(
					shaderHandle,
					shader->locs["lightColour"],
					wf::WHITE
				);
				wf::wgl::setShaderUniform(
					shaderHandle,
					shader->locs["ambientLevel"],
					0.5f
				);

				glDisable(GL_DEPTH_TEST);

				// @todo
			/*	setCullMode(material->cullMode);
				setBlendMode(material->blendMode);
				setDepthSettings(material->depthMask, material->depthTest, material->depthFunc);*/

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

		wf::getWindow().swapBuffers();
	}

	void CoreSystem::teardown()
	{
		// delete material (i.e. textures and shaders)
		entityManager->each<Component::Material>(
			[&](Component::Material& material) {
				if (!material.shader->handle) return;
				if (material.diffuse.map && material.diffuse.map->handle) {
					wf::wgl::destroyTexture(*material.diffuse.map.get());
				}
				if (material.normal.map && material.normal.map->handle) {
					wf::wgl::destroyTexture(*material.normal.map.get());
				}
				if (material.specular.map && material.specular.map->handle) {
					wf::wgl::destroyTexture(*material.specular.map.get());
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
			// flags
			material.shader->locs["hasDiffuseMap"] = wf::wgl::getShaderUniformLocation(shader, "hasDiffuseMap");
			material.shader->locs["hasNormalMap"] = wf::wgl::getShaderUniformLocation(shader, "hasNormalMap");
			material.shader->locs["hasSpecularMap"] = wf::wgl::getShaderUniformLocation(shader, "hasSpecularMap");
			// material props
			material.shader->locs["diffuseColour"] = wf::wgl::getShaderUniformLocation(shader, "diffuseColour");
			material.shader->locs["specularColour"] = wf::wgl::getShaderUniformLocation(shader, "specularColour");
			material.shader->locs["normalStrength"] = wf::wgl::getShaderUniformLocation(shader, "normalStrength");
			material.shader->locs["specularShininess"] = wf::wgl::getShaderUniformLocation(shader, "specularShininess");
			material.shader->locs["specularIntensity"] = wf::wgl::getShaderUniformLocation(shader, "specularIntensity");
			// lights & camera
			material.shader->locs["viewPos"] = wf::wgl::getShaderUniformLocation(shader, "viewPos");
			material.shader->locs["lightDir"] = wf::wgl::getShaderUniformLocation(shader, "lightDir");
			material.shader->locs["lightColour"] = wf::wgl::getShaderUniformLocation(shader, "lightColour");
			material.shader->locs["ambientLevel"] = wf::wgl::getShaderUniformLocation(shader, "ambientLevel");
		}

		if (material.diffuse.map && !material.diffuse.map->handle) {
			uploadMaterialTexture(*material.diffuse.map.get());
		}

		if (material.normal.map && !material.normal.map->handle) {
			uploadMaterialTexture(*material.normal.map.get());
		}

		if (material.specular.map && !material.specular.map->handle) {
			uploadMaterialTexture(*material.specular.map.get());
		}
	}

	void CoreSystem::updateMaterialData(const Component::Material& material)
	{
		if (!material.shader || !material.shader->handle) throw std::runtime_error("NO shader loaded");

		auto& shader = material.shader;
		auto shaderHandle = m_shaders[shader->handle];

		wf::wgl::useShader(shaderHandle);

		// DIFFUSE
		if (shader->isValidLocation("hasDiffuseMap") && shader->isValidLocation("diffuseMap")) {
			wf::wgl::setShaderUniform(shaderHandle, shader->locs["hasDiffuseMap"], material.hasDiffuseTexture());

			if (material.hasDiffuseTexture()) {
				wf::wgl::bindTexture(*material.diffuse.map.get(), 0);
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
				wf::wgl::bindTexture(*material.normal.map.get(), 1);
				wf::wgl::setShaderUniform(
					shaderHandle,
					shader->locs["diffuseMap"],
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
				wf::wgl::bindTexture(*material.specular.map.get(), 2);
				wf::wgl::setShaderUniform(
					shaderHandle,
					shader->locs["specularMap"],
					2
				);
			}
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

	void CoreSystem::uploadMaterialTexture(wf::Texture& texture)
	{
		// @todo upload the texture
		// @todo update the param with the handle
	}
}