#pragma once
#include "Engine.h"

namespace Sandbox
{
	namespace Component
	{
		struct Geometry;
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

	private:
		std::unordered_map<unsigned int, wf::wgl::MeshBuffers> m_meshBuffers;

		unsigned int m_lastId{ 0 };
	};
}