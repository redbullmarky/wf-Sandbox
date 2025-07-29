#pragma once
#include "Engine.h"

namespace Sandbox
{
	namespace Component
	{
		struct Geometry;
		struct Material;
	}

	class CoreSystem : public wf::ISystem
	{
	public:
		CoreSystem(wf::Scene* scene);
		~CoreSystem() = default;

		virtual bool init() override;
		virtual void update(float dt) override;
		virtual void render(float dt) override;
		virtual void renderGui(float dt) override;
		virtual void teardown() override;

	private:
		void uploadMesh(Component::Geometry& geometry);
		void updateMeshData(const Component::Geometry& geometry);

		void uploadMaterialData(Component::Material& material);
		void updateMaterialData(const Component::Material& material);

	private:
		std::unordered_map<unsigned int, wf::wgl::MeshBufferHandle> m_meshBuffers;
		std::unordered_map<unsigned int, wf::wgl::TextureHandle> m_textures;
		std::unordered_map<unsigned int, wf::wgl::ShaderHandle> m_shaders;

		unsigned int m_lastId{ 0 };
	};
}