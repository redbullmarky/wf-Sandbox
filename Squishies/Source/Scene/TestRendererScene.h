#pragma once
#include "Engine.h"

namespace Squishies
{
	class TestRendererScene : public wf::Scene
	{
	public:
		TestRendererScene() = default;
		~TestRendererScene() = default;

		virtual bool init() override;
		virtual void setup() override;
		virtual void teardown() override;
		virtual void render(float dt) override;

	private:
		void prepareScene();

	private:
		wf::wgl::RenderTargetHandle m_shadowMap;
	};
}