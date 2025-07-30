#pragma once
#include "Engine.h"

namespace Squishies
{
	class GameScene : public wf::Scene
	{
	public:
		GameScene() = default;
		~GameScene() = default;

		virtual bool init() override;
		virtual void setup() override;
		virtual void teardown() override;
		virtual void renderGui(float dt) override;
	};
}