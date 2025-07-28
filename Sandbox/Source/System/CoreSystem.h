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
		virtual void teardown() override;

	private:
		void uploadMesh(Component::Geometry& geometry);
		void updateMeshData(const Component::Geometry& geometry);

		void uploadMaterialData(Component::Material& material);
		void updateMaterialData(const Component::Material& material);

		void uploadMaterialTexture(wf::Texture& texture);

	private:
		std::unordered_map<unsigned int, wf::wgl::MeshBuffers> m_meshBuffers;
		std::unordered_map<unsigned int, unsigned int> m_shaders;

		unsigned int m_lastId{ 0 };
	};
}