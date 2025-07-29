#pragma once
#include "Engine.h"

#include "Core/BaseScene.h"

namespace Sandbox
{
	class GameScene : public BaseScene
	{
	public:
		GameScene() = default;
		~GameScene() = default;

		virtual void setup() override;
		virtual void teardown() override;
		virtual void render(float dt) override;
		virtual void renderGui(float dt) override;

	private:
		void prepareScene();

	public:
		wf::component::Camera m_lightCamera;

	private:
		wf::wgl::RenderTargetHandle m_shadowMap;
	};
}