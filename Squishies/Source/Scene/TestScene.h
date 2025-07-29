#pragma once
#include "Engine.h"

namespace Squishies
{
	class TestScene : public wf::Scene
	{
	public:
		TestScene() = default;
		~TestScene() = default;

		virtual bool init() override;
		virtual void setup() override;
		virtual void teardown() override;
		virtual void render(float dt) override;
		virtual void renderGui(float dt) override;

	private:
		void prepareScene();

	private:
		wf::wgl::RenderTargetHandle m_shadowMap;
	};
}