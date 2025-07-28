#pragma once
#include "Scene.h"

namespace Sandbox
{
	class GameScene : public Scene
	{
	public:
		GameScene() = default;
		~GameScene() = default;

		virtual bool init() override;
		virtual void shutdown() override;

		virtual void update(float dt) override;
		virtual void fixedUpdate(float dt) override;
		virtual void render(float dt) override;

	private:
		void prepareScene();
	};
}