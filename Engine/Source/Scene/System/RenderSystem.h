#pragma once
#include "Scene/Scene.h"
#include "Scene/System.h"

namespace wf::system
{
	class RenderSystem : public ISystem
	{
	public:
		RenderSystem(Scene* scene) : ISystem(scene) {}
		~RenderSystem() = default;

		virtual bool init() override { return true; }
		virtual void update(float dt) override;
		virtual void render(float dt) override;
		virtual void teardown() override;
	};
}