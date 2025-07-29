#pragma once
#include "Engine.h"
#include <memory>

namespace Squishies
{
	class TestScene;

	class Squishies : public wf::Application
	{
	public:
		Squishies() = default;
		~Squishies() = default;

		virtual bool init() override;
		virtual void run() override;
		virtual void shutdown() override;

	private:
		std::shared_ptr<TestScene> m_scene;
	};
}