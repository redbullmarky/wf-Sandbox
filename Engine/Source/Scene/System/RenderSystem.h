#pragma once
#include "Core/GL.h"
#include "Scene/Scene.h"
#include "Scene/System.h"

#include <unordered_map>

namespace wf
{
	struct GeometryComponent;
	struct MaterialComponent;
}

namespace wf::system
{
	class RenderSystem : public ISystem
	{
	public:
		RenderSystem(Scene* scene);
		~RenderSystem() = default;

		virtual bool init() override;
		virtual void update(float dt) override;
		virtual void render(float dt) override;
		virtual void teardown() override;

	private:
		void uploadMesh(GeometryComponent& geometry);
		void updateMeshData(const GeometryComponent& geometry);

		void uploadMaterialData(MaterialComponent& material);
		void updateMaterialData(const MaterialComponent& material);

	private:
		std::unordered_map<unsigned int, wgl::MeshBufferHandle> m_meshBuffers;
		std::unordered_map<unsigned int, wgl::TextureHandle> m_textures;
		std::unordered_map<unsigned int, wgl::ShaderHandle> m_shaders;

		unsigned int m_lastId{ 0 };
	};
}