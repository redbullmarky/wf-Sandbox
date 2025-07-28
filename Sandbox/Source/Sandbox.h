#pragma once
#include "Engine.h"
#include <memory>

namespace Sandbox
{
	class GameScene;

	class Sandbox : public wf::Application
	{
	public:
		Sandbox() = default;
		~Sandbox() = default;

		virtual bool init() override;
		virtual void run() override;
		virtual void shutdown() override;

	private:
		std::shared_ptr<GameScene> m_scene;
	};
}