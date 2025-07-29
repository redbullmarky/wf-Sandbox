#pragma once
#include "Core/BaseScene.h"

namespace Sandbox
{
	class GameScene : public BaseScene
	{
	public:
		GameScene() = default;
		~GameScene() = default;

		virtual void setup() override;
		virtual void renderGui(float dt) override;

	private:
		void prepareScene();
	};
}