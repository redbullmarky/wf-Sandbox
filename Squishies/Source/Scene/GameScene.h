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
		virtual void renderGui(float dt) override;

	private:
		void resetSquishies();
		wf::Entity createSquishy(const std::string& name, const wf::Vec3 pos, const wf::Colour& colour);
		void resetInventory(wf::Entity entity);

	private:
		bool m_debug{ true };
	};
}