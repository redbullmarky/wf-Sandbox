#pragma once
#include "Core/BaseScene.h"

namespace Sandbox
{
	class GameScene : public BaseScene
	{
	public:
		GameScene() = default;
		~GameScene() = default;

		virtual bool init() override;
		virtual void shutdown() override;

		virtual void update(float dt) override;

	private:
		void prepareScene();
	};
}