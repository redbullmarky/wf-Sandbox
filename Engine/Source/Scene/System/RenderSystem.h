#pragma once
#include "Core/GL.h"
#include "Render/Material.h"
#include "Scene/Scene.h"
#include "Scene/System.h"

namespace wf
{
	struct GeometryComponent;
	struct MeshRendererComponent;
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

		void uploadMaterialData(Material& material);
	};
}