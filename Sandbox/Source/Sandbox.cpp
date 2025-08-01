#include "Sandbox.h"
#include "Engine.h"

#include "Scene/TestScene.h"

namespace Sandbox
{
	bool Sandbox::init()
	{
		if (!wf::init("Sandbox", 1600, 900)) {
			return false;
		}

		wf::initGui();

		m_scene = std::make_shared<TestScene>();
		if (!m_scene->init()) {
			return false;
		}

		m_scene->setup();

		return true;
	}

	void Sandbox::run()
	{
		while (!wf::shouldClose()) {
			while (wf::isFixedUpdateReady()) {
				m_scene->fixedUpdate(wf::getFixedTimestep());
			}
			m_scene->update(wf::getDeltaTime());

			if (wf::beginDrawing()) {
				m_scene->render(wf::getDeltaTime());

				if (wf::beginGui()) {
					m_scene->renderGui(wf::getDeltaTime());
					wf::endGui();
				}

				wf::endDrawing();
			}
		}
	}

	void Sandbox::shutdown()
	{
		m_scene->shutdown();
		wf::shutdownGui();
		wf::shutdown();
	}
}