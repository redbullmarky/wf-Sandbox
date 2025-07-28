#include "Sandbox.h"
#include "Engine.h"

#include "Scene/GameScene.h"

namespace Sandbox
{
	bool Sandbox::init()
	{
		if (!wf::init("Sandbox", 1600, 900)) {
			return false;
		}

		wf::initGui();

		m_scene = std::make_shared<GameScene>();
		if (!m_scene->init()) {
			return false;
		}

		return true;
	}

	void Sandbox::run()
	{
		while (!wf::shouldClose()) {
			while (wf::isFixedUpdateReady()) {
				m_scene->fixedUpdate(wf::getFixedTime());
			}
			m_scene->update(wf::getDeltaTime());
			m_scene->render(wf::getDeltaTime());
		}
	}

	void Sandbox::shutdown()
	{
		m_scene->shutdown();
		wf::shutdownGui();
		wf::shutdown();
	}
}